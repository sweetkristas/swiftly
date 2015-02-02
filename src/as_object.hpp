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

    struct as_object_def
    {
		virtual ~as_object_def() {}
        virtual bool is_a(ASClass id) = 0;
		virtual void init() {}
    };

	class as_object : public as_object_def
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
		virtual void set_property(const std::string& name, const as_value_ptr& value);
		virtual as_value_ptr get_property(const std::string& name);

		virtual void clone_display_object(const std::string& newname, int depth) {}
		virtual void remove_display_object(const as_object_ptr& obj) {}
		virtual as_object_ptr get_parent() { return nullptr; }
		as_object_ptr find_target(const as_value_ptr& value);
		as_object_ptr find_target(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end);

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

	protected:
		explicit as_object(weak_player_ptr player) : player_(player) {}
	private:
		weak_player_ptr player_;
	};

	bool path_parse(const std::string& path, std::vector<std::string>* parts);
}
