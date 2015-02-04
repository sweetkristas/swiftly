#include "asserts.hpp"
#include "swf_character.hpp"
#include "swf_command.hpp"

namespace swf
{
	command::command()
	{
	}
	
	command::~command()
	{
	}

	set_background_color::set_background_color(rgba color) 
		: color_(color) 
	{
		LOG_DEBUG("set_background_color: " << color);
	}
	
	set_background_color::set_background_color(rgb color)
		: color_()
	{
		color_.r = color.r;
		color_.g = color.g;
		color_.b = color.b;
		LOG_DEBUG("set_background_color: " << color_);
	}
	
	void set_background_color::execute(const character_ptr& ch)
	{
		ch->set_background_color(color_);
	}

	place_object::place_object(int id, int depth, const matrix2x3& m, bool has_color_transform, const color_transform& ct)
		: pp_(depth)
	{
		LOG_DEBUG("place_object: " << id << ", depth: " << depth);
		pp_.set_id(id);
		pp_.set_matrix_transform(m);
		if(has_color_transform) {
			pp_.set_color_transform(ct);
		}
	}

	void place_object::execute(const character_ptr& ch) 
	{
		ch->add_to_display_list(pp_);
	}

	place_object2::place_object2(Operation op, const placement_params& pp) 
		: op_(op), 
		  pp_(pp) 
	{
		LOG_DEBUG("place_object2 " << pp);
	}

	void place_object2::execute(const character_ptr& ch) 
	{
		switch(op_)
		{
		case Operation::PLACE:
			ch->add_to_display_list(pp_);
			break;
		case Operation::MOVE:
			LOG_ERROR("place_object2: case Operation::MOVE");
			break;
		case Operation::REPLACE:
			LOG_ERROR("place_object2: case Operation::REPLACE");
			break;
		default: break;
		}
	}

	remove_object2::remove_object2(int depth) 
		: depth_(depth) 
	{
		LOG_DEBUG("remove_object2 depth:" << depth);
	}

	void remove_object2::execute(const character_ptr& ch) 
	{
		ch->remove_from_display_list(depth_);
	}

	do_action::do_action(const action_ptr& actions)
		: actions_(actions)
	{
		LOG_DEBUG("do_action");
	}

	void do_action::execute(const character_ptr& ch)
	{
		ASSERT_LOG(actions_ != nullptr, "Actions were empty.");
		actions_->execute(ch, false);
	}
}
