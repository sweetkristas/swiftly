#pragma once

#include "swf_character.hpp"

namespace swf
{
	class sprite : public character
	{
	public:
		sprite(player_ptr player, weak_character_ptr parent, int id, movie_def_ptr def);

		virtual bool is_a(ASClass id) override { return id == ASClass::SPRITE; }

		virtual bool is_tag_allowable(Tag tag) override;
	private:
		movie_def_ptr def_;
		// std::vector<action_ptr> actions_;
		// std::vector<action_ptr> init_actions_;
		// bool done_init_actions_;
	};

	typedef std::shared_ptr<sprite> sprite_ptr;
}
