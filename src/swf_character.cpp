#include <algorithm>
#include "swf_character.hpp"
#include "swf_player.hpp"

namespace swf
{
	character_def::character_def()
		: as_object_def()
	{
	}

	character_def::~character_def()
	{
	}

	void character_def::add_symbol_class(int character_id, const std::string& class_name)
	{
		as3_symbol_map_[character_id] = class_name;
	}

	void character_def::set_registered_class_constructor(as_function_ptr fn)
	{
		ASSERT_LOG(false, "XXX character_def::set_registered_class_constructor");
	}

	character::character(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: as_object(player),
		  parent_(parent),
		  id_(id),
		  def_(def),
		  background_color_(),
		  display_list_(),
		  play_state_(PlayState::PLAYING),
		  high_quality_(true),
		  mat_(),
		  ct_(),
		  morph_ratio_(0),
		  clip_depth_(0),
		  name_(),
		  environment_(environment::create(player))
	{
	}

	character::~character()
	{
	}

	void character::set_background_alpha(int a)
	{
		background_color_.a = std::min(255, std::max(0, a));
	}

	void character::call_frame_actions(const as_value_ptr& val)
	{
		ASSERT_LOG(false, "It's probably a mistake to do call_frame_actions on this character.");
	}

	void character::set_background_color(const rgba& color)
	{
		background_color_ = color;
	}

	void character::add_to_display_list(const placement_params& pp)
	{
		auto it = display_list_.find(pp.get_depth());
		if(it != display_list_.end()) {
			LOG_WARN("Adding character at depth " << pp.get_depth() << " replaces existing character.");
		}
		ASSERT_LOG(pp.has_id(), "Adding character at depth: " << pp.get_depth() << "But has no id.");
		auto ch = get_player()->get_root_movie()->get_character_from_id(get_current_frame(), pp.get_id());
		display_list_[pp.get_depth()] = ch;

		// Execute any frame 0 actions.
		ch->get_definition()->execute_commands(0, ch);
	}

	void character::remove_from_display_list(int depth)
	{
		auto it = display_list_.find(depth);
		ASSERT_LOG(it != display_list_.end(), "Removing item from display list at depth: " << depth << " and no item found.");
		display_list_.erase(it);
	}

	void character::modify_display_list(const placement_params& pp)
	{
		auto it = display_list_.find(pp.get_depth());
		ASSERT_LOG(it != display_list_.end(), "Modifying item on display list at depth: " << pp.get_depth() << " and no item found.");
		it->second->modify_character_params(pp);
	}

	void character::modify_character_params(const placement_params& pp)
	{
		if(pp.has_id()) {
			// ignore if we have an id, since modifying the character id would be instantiating a new character.
		}
		if(pp.has_matrix_transform()) {
			mat_ = pp.get_matrix_transform();
		}
		if(pp.has_color_transform()) {
			ct_ = pp.get_color_transform();
		}
		if(pp.has_morph_ratio()) {
			morph_ratio_ = pp.get_morph_ratio();
		}
		if(pp.has_clip_depth()) { 
			clip_depth_ = pp.get_clip_depth();
		}
		if(pp.has_name()) {
			name_ = pp.get_name();
		}
		if(pp.has_clip_actions()) {
			clip_actions_ = pp.get_clip_actions();
		}
	}

	void character::update_display_list(float delta_time)
	{
		for(auto& ch : display_list_) {
			ch.second->update(delta_time);
		}
	}

	character_ptr character::get_character_ptr()
	{
		// XXX I really don't like this here. but c'est la vie. 
		return std::dynamic_pointer_cast<character>(shared_from_this());
	}

	character_ptr character::get_named_character(const std::string& name)
	{
		if(name == name_) {
			return get_character_ptr();
		}
		for(auto& ch : display_list_) {
			if(ch.second->get_name() == name) {
				return ch.second;
			}
			auto ptr = ch.second->get_named_character(name);
			if(ptr) {
				return ptr;
			}
		}
		return nullptr;
	}

	void character::stop_sounds()
	{
		LOG_ERROR("XXX do stop_sounds here");
	}

	void character::draw() const
	{
		handle_draw();
		for(auto& ch : display_list_) {
			ch.second->draw();
		}
	}

	void character::clone_display_object(const std::string& newname, int depth)
	{
		auto new_obj = get_definition()->create_instance(get_player(), parent_.lock(), get_id());
		placement_params pp(depth);
		if(!clip_actions_.clip_records.empty()) {
			pp.set_clip_actions(clip_actions_);
		}
		pp.set_name(newname);
		pp.set_matrix_transform(mat_);
		pp.set_color_transform(ct_);
		if(morph_ratio_ != 0) {
			pp.set_morph_ratio(morph_ratio_);
		}
		if(clip_depth_ != 0) {
			pp.set_clip_depth(clip_depth_);
		}
		new_obj->add_to_display_list(pp);
	}

	void character::remove_display_object(const as_object_ptr& obj)
	{
		for(auto& ch : display_list_) {
			if(obj == ch.second) {
				remove_from_display_list(ch.first);
			}
		}
		ASSERT_LOG(false, "Couldn't find object on display_list.");
	}

	as_object_ptr character::get_parent()
	{
		return parent_.lock();
	}
}
