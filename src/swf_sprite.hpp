#pragma once

#include "swf_character.hpp"
#include "swf_command.hpp"
#include "swf_movie.hpp"

namespace swf
{
	class sprite_def : public movie_def
	{
	public:
		MAKE_FACTORY(sprite_def);
		virtual ~sprite_def() {}

		virtual bool is_a(ASClass id) override { return id == ASClass::SPRITE_DEF ? true : movie_def::is_a(id); }
		virtual bool sprite_def::is_tag_allowable(Tag tag) override;
	private:
		sprite_def(int id, int frame_count);
		void init();
		
		int id_;

		// std::vector<action_ptr> actions_;
		// std::vector<action_ptr> init_actions_;
		// bool done_init_actions_;
	};

	class sprite : public movie
	{
	public:
		MAKE_FACTORY(sprite);
		virtual ~sprite() {}

		virtual bool is_a(ASClass id) override { return id == ASClass::SPRITE ? true : movie::is_a(id); }
	private:
		sprite(const weak_player_ptr& player, const character_ptr& parent, int i, character_def_ptr def);
	};

	typedef std::shared_ptr<sprite_def> sprite_def_ptr;
}
