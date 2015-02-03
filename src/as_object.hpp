#pragma once

#include "as_fwd.hpp"
#include "swf_fwd.hpp"
#include "swf_types.hpp"

namespace swf
{
    enum class ASClass
    {
        OBJECT,
        CHARACTER,
        MOVIE,
        SPRITE,
        FUNCTION,
        C_FUNCTION,
        S_FUNCTION,
        AS3_FUNCTION,
        CHARACTER_DEF,
        SPRITE_DEF,
        MOVIE_DEF,
        VIDEO_DEF,
        COLOR,
        SOUND,
		FONT_DEF,
        FONT,
		SHAPE_DEF,
		SHAPE,
		BUTTON_DEF,
		BUTTON,
		TEXT,
		TEXT_DEF,
		EDIT_TEXT,
		EDIT_TEXT_DEF,
		IMAGE,
		IMAGE_DEF,
        // etc
    };

	struct as_watch {
		as_watch() : callback_(), arg_() {}
		as_watch(const as_function_ptr& cb, const as_value_ptr& arg) : callback_(cb), arg_(arg) {}
		as_function_ptr callback_;
		as_value_ptr arg_;
	};

    struct as_object_def
    {
		virtual ~as_object_def() {}
        virtual bool is_a(ASClass id) = 0;
		virtual void init() {}
		virtual as_object_def_ptr find_exported_resources(const std::string& name) { return nullptr; }
		virtual void set_registered_class_constructor(as_function_ptr fn) {}
    };

	class as_object : public as_object_def, public std::enable_shared_from_this<as_object>
	{
	public:
		MAKE_FACTORY(as_object);
		virtual ~as_object() {}

		virtual bool is_a(ASClass id) override  {
			return id == ASClass::OBJECT;
		}		

		player_ptr get_player() { 
			auto player = player_.lock();
			ASSERT_LOG(player != nullptr, "Player went away in call to as_object::get_player()");
			return player;
		}

		virtual const char*	to_string() { return "[object Object]"; }
		virtual double to_number();
		virtual bool to_bool() { return true; }
		virtual as_value default_value(HintType hint=HintType::NO_HINT);

		virtual as_value_ptr get_member(const std::string& name);
		virtual void set_member(const std::string& name, const as_value_ptr& value);
		void set_builtin_member(const std::string& name, const as_value_ptr& value);

		virtual void clone_display_object(const std::string& newname, int depth) {}
		virtual void remove_display_object(const as_object_ptr& obj) {}
		virtual as_object_ptr get_parent() { return nullptr; }
		as_object_ptr find_target(const as_value_ptr& value);
		as_object_ptr find_target(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end);
		
		as_object_ptr get_prototype() { return prototype_; }
		as_value_ptr get_constructor() const;
		void set_constructor(const as_value_ptr& ctor);

		virtual void next_frame() {}
		virtual void prev_frame() {}
		virtual int get_current_frame() const { return -1; }
		virtual void set_current_frame(int frame) {}
		virtual void stop_sounds() {}
		virtual bool wait_frame(int frame) { return false; }
		virtual void set_play_state(PlayState state) {}
		virtual void toggle_quality() {}
		character_def_ptr get_definition() { return nullptr; }
		virtual void call_frame_actions(const as_value_ptr& val) {}

		virtual environment_ptr get_environment() { return nullptr; }

		virtual bool set_watch(const std::string& property_name, as_function_ptr callback, as_value_ptr argument);
		virtual bool remove_watch(const std::string& property_name);
		virtual bool call_watcher(const std::string& name, as_value_ptr old_value, as_value_ptr new_value);

		// Built-in functions
		static as_value_ptr add_property(const function_params& params);
		static as_value_ptr register_class(const function_params& params);
		static as_value_ptr has_own_property(const function_params& params);
		static as_value_ptr watch(const function_params& params);
		static as_value_ptr unwatch(const function_params& params);
		static as_value_ptr add_event_listener(const function_params& params);
		static as_value_ptr dump(const function_params& params);

		as_value_ptr get_this_value();
	protected:
		explicit as_object(weak_player_ptr player) : player_(player) {}
	private:
		weak_player_ptr player_;
		as_object_ptr prototype_;
		std::map<std::string, as_value_ptr> members_;

		std::map<std::string, as_watch> watchers_;
	};

	bool path_parse(const std::string& path, std::vector<std::string>* parts);
}
