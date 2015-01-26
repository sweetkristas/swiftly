#pragma once

#include <memory>

namespace swf
{
	class player;
	typedef std::shared_ptr<player> player_ptr;
	typedef std::weak_ptr<player> player_weak_ptr;
}