#include "swf_sprite.hpp"

namespace swf
{
	sprite::sprite(const weak_player_ptr& player, const character_ptr& parent, int id, character_def_ptr def)
		: character(player, parent, id, def)
	{
	}

	sprite_def::sprite_def(int id, int frame_count)
		: id_(id),
		  frame_count_(frame_count),
		  current_frame_(0)
	{
	}

	bool sprite_def::is_tag_allowable(Tag tag)
	{
		switch(tag) {
		case Tag::SHOW_FRAME:
		case Tag::PLACE_OBJECT:
		case Tag::PLACE_OBJECT2:
		case Tag::PLACE_OBJECT3:
		case Tag::REMOVE_OBJECT:
		case Tag::REMOVE_OBJECT2:
		case Tag::START_SOUND:
		case Tag::FRAME_LABEL:
		case Tag::SOUND_STREAM_HEAD:
		case Tag::SOUND_STREAM_HEAD2:
		case Tag::SOUND_STREAM_BLOCK:
		case Tag::END:
		case Tag::DO_ACTION:
		case Tag::DO_INIT_ACTION:
		case Tag::DO_ABC:
			return true;
		default: break;
		}
		return false;
	}

	void sprite_def::init()
	{
		commands_.resize(frame_count_);
	}

	void sprite_def::show_frame()
	{
		ASSERT_LOG(false, "XXX sprite_def::show_frame");
	}

	void sprite_def::add_character(int id, const character_def_ptr& ch)
	{
		ASSERT_LOG(false, "XXX sprite_def::add_character");
	}

	void sprite_def::add_command(const command_ptr& cmd)
	{
		ASSERT_LOG(get_current_frame() < static_cast<int>(commands_.size()), "Current frame exceeds the number of frames defined.");
		commands_[get_current_frame()].emplace_back(cmd);
	}

	character_ptr sprite_def::get_character_from_id(int id)
	{
		ASSERT_LOG(false, "XXX sprite_def::get_character_from_id");
		return nullptr;
	}

	void sprite_def::add_frame_label(unsigned frame, const std::string& label)
	{
		ASSERT_LOG(false, "Frame labels and scene information is supported for main movie only.");
	}

	void sprite_def::add_scene_info(unsigned frame, const std::string& label)
	{
		ASSERT_LOG(false, "Frame labels and scene information is supported for main movie only.");
	}
}
