#pragma once

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
	protected:
		explicit as_object(weak_player_ptr player) : player_(player) {}
	private:
		weak_player_ptr player_;
	};
}
