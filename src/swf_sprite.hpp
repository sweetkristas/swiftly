#pragma once

#include "swf_character.hpp"
#include "swf_command.hpp"

namespace swf
{
	class sprite_def : public character_def
	{
	public:
		MAKE_FACTORY(sprite_def);
		virtual ~sprite_def() {}

		virtual bool is_a(ASClass id) override { return id == ASClass::SPRITE_DEF ? true : character_def::is_a(id); }
		virtual bool sprite_def::is_tag_allowable(Tag tag) override;

		int get_current_frame() const { return current_frame_; }

		void show_frame() override;

		void add_character(int id, const character_def_ptr& ch) override;
		void add_command(const command_ptr& cmd) override;

		character_ptr get_character_from_id(int id) override;
		void add_frame_label(unsigned frame, const std::string& label) override;
		void add_scene_info(unsigned frame, const std::string& label) override;
	private:
		sprite_def(int id, int frame_count);
		void init();
		int id_;
		int frame_count_;

		int current_frame_;
		std::vector<std::vector<command_ptr>> commands_;

		// std::vector<action_ptr> actions_;
		// std::vector<action_ptr> init_actions_;
		// bool done_init_actions_;
	};

	class sprite : public character
	{
	public:
		MAKE_FACTORY(sprite);
		virtual ~sprite() {}

		virtual bool is_a(ASClass id) override { return id == ASClass::SPRITE ? true : character::is_a(id); }
	private:
		sprite(const weak_player_ptr& player, const character_ptr& parent, int i, character_def_ptr def);
	};

	typedef std::shared_ptr<sprite_def> sprite_def_ptr;
}
