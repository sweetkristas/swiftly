#pragma once

#include "as_object.hpp"
#include "swf_command.hpp"
#include "swf_params.hpp"
#include "swf_tags.hpp"

namespace swf
{
	class character_def : public as_object_def, public std::enable_shared_from_this<character_def>
	{
	public:
		character_def();
		virtual ~character_def();

		void add_symbol_class(int character_id, const std::string& class_name);

		virtual bool is_a(ASClass id) override { return id == ASClass::CHARACTER_DEF; }
		virtual bool is_tag_allowable(Tag tag) { return true; }

		virtual void show_frame() {};
		virtual int get_frame_count() const { return 0; }
		virtual void add_character(int id, const character_def_ptr& ch) {};
		virtual void add_command(const command_ptr& cmd) {};
		virtual character_ptr get_character_from_id(int id) { return nullptr; };
		virtual void set_frame_label(const std::string& label) {};
		virtual void add_frame_label(unsigned frame, const std::string& label) {};
		virtual void add_scene_info(unsigned frame, const std::string& label) {};

		virtual void execute_commands(int frame, const character_ptr& ch) {};

		virtual character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id);
private:
		std::map<int, std::string> as3_symbol_map_;
	};

	typedef std::map<int, character_ptr> display_list;

	class character : public as_object, public std::enable_shared_from_this<character>
	{
	public:
		explicit character(const weak_player_ptr& player, const character_ptr& parent, int i, const character_def_ptr& def);
		virtual ~character();

		virtual bool is_a(ASClass id) override { return id == ASClass::CHARACTER; }

		character_def_ptr get_definition() { return def_; }

		void set_background_color(const rgba& color);
		const rgba& get_background_color() const { return background_color_; }

		void set_background_alpha(int a=255);

		void add_to_display_list(const placement_params& params);
		void remove_from_display_list(int depth);
		void modify_display_list(const placement_params& params);
		void modify_character_params(const character_ptr& ch, const placement_params& params);

		virtual void next_frame() {}
		virtual void prev_frame() {}

		virtual void update(float delta_time) {}
		virtual void draw() const {}
	private:		
		weak_character_ptr parent_;
		int id_;
		character_def_ptr def_;
		rgba background_color_;
		display_list display_list_;
	};
}
