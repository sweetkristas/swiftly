#pragma once

#include "swf_player_fwd.hpp"

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
        FONT,
        // etc
    };

    struct as_object_def
    {
		virtual ~as_object_def() {}
        virtual bool is(ASClass id) = 0;
    };

	class as_object : public as_object_def
	{
	public:
		as_object(player_ptr player) : player_(player) {}
		virtual ~as_object() {}

		virtual bool is(ASClass id) override  {
			return id == ASClass::OBJECT;
		}		
	private:
		player_ptr player_;
	};
}
