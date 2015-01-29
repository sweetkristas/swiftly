#include "swf_sprite.hpp"

namespace swf
{
	sprite::sprite(const weak_player_ptr& player, const character_ptr& parent, int id, character_def_ptr def)
		: movie(player, parent, id, def)
	{
	}

	sprite_def::sprite_def(int id, int frame_count)
		: id_(id)
	{
		set_frame_count(frame_count);
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
	}
}
