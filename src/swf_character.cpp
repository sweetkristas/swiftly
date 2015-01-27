#include <algorithm>
#include "swf_character.hpp"

namespace swf
{
	character_def::character_def()
		: as_object_def()
	{
	}

	character_def::~character_def()
	{
	}

	character::character(player_ptr player, weak_character_ptr parent, int id)
		: as_object(player),
		  parent_(parent),
		  id_(id),
		  background_color_()
	{
	}

	character::~character()
	{
	}

	void character::set_background_alpha(int a)
	{
		background_color_.a = std::min(255, std::max(0, a));
	}

	void character::set_background_color(const rgba& color)
	{
		background_color_ = color;
	}
}
