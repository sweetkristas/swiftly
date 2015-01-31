#pragma once

#include "as_object.hpp"
#include "swf_command.hpp"
#include "swf_environment.hpp"
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
		virtual character_def_ptr get_character_def_from_id(int frame, int id) { return nullptr; };
		virtual void set_frame_label(const std::string& label) {};
		virtual int get_frame_from_label(const std::string& label) { return 0; }
		virtual void add_frame_label(unsigned frame, const std::string& label) {};
		virtual void add_scene_info(unsigned frame, const std::string& label) {};

		virtual void execute_commands(int frame, const character_ptr& ch, bool actions_only=false) {};

		virtual character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id) = 0;
		//template<typename T>
		//character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id) {
		//	return typename T::create(player, parent, id, shared_from_this());
		//}

private:
		std::map<int, std::string> as3_symbol_map_;
	};

	typedef std::map<int, character_ptr> display_list;

	enum class PlayState
	{
		PLAYING,
		STOPPED,
	};

	class character : public as_object, public std::enable_shared_from_this<character>
	{
	public:
		explicit character(const weak_player_ptr& player, const character_ptr& parent, int i, const character_def_ptr& def);
		virtual ~character();

		virtual bool is_a(ASClass id) override { return id == ASClass::CHARACTER; }

		character_def_ptr get_definition() { return def_; }
		int get_id() const { return id_; }

		void set_background_color(const rgba& color);
		const rgba& get_background_color() const { return background_color_; }

		void set_background_alpha(int a=255);

		void add_to_display_list(const placement_params& params);
		void remove_from_display_list(int depth);
		void modify_display_list(const placement_params& params);
		void modify_character_params(const placement_params& params);
		void update_display_list(float delta_time);
		void clone_display_object(const std::string& newname, int depth);
		virtual void remove_display_object(const as_object_ptr& obj);
		virtual as_object_ptr get_parent();

		virtual void next_frame() {}
		virtual void prev_frame() {}
		virtual int get_current_frame() const { return -1; }
		virtual void set_current_frame(int frame) {}
		void stop_sounds();
		// Default way to do this in a single-threaded environment is to ignore it.
		virtual bool wait_frame(int frame) { return false; }

		PlayState get_play_state() const { return play_state_; }
		void set_play_state(PlayState state) { play_state_ = state; }

		void toggle_quality() { high_quality_ = !high_quality_; }

		virtual void update(float delta_time) {}
		void draw() const;

		const std::string& get_name() const { return name_; }

		character_ptr get_named_character(const std::string& name);
		virtual void call_frame_actions(const as_value_ptr& val);

		environment& get_environment() { return environment_; }
	private:
		virtual void handle_draw() const = 0;
		weak_character_ptr parent_;
		int id_;
		character_def_ptr def_;
		rgba background_color_;
		display_list display_list_;
		PlayState play_state_;
		bool high_quality_;

		matrix2x3 mat_;
		color_transform ct_;
		int morph_ratio_;
		int clip_depth_;
		std::string name_;
		clip_actions clip_actions_;

		environment environment_;
	};
}
