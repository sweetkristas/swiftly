#pragma once

#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

#include "geometry.hpp"

#define MAKE_FACTORY(classname)																\
	template<typename... T>																	\
	static std::shared_ptr<classname> create(T&& ... all) {									\
		auto bptr = std::shared_ptr<classname>(new classname(std::forward<T>(all)...));		\
		bptr->init();																		\
		return bptr;																		\
	}

namespace swf
{
	enum class PlayState
	{
		PLAYING,
		STOPPED,
	};

	class player;
	typedef std::shared_ptr<player> player_ptr;
	typedef std::weak_ptr<player> weak_player_ptr;

	class movie_def;
	typedef std::shared_ptr<movie_def> movie_def_ptr;
	//typedef std::shared_ptr<const movie_def> const_movie_def_ptr;
	class movie;
	typedef std::shared_ptr<movie> movie_ptr;

	class as_object;
	typedef std::shared_ptr<as_object> as_object_ptr;

	class character;
	typedef std::shared_ptr<character> character_ptr;
	typedef std::shared_ptr<const character> const_character_ptr;
	typedef std::weak_ptr<character> weak_character_ptr;
	class character_def;
	typedef std::shared_ptr<character_def> character_def_ptr;
	//typedef std::shared_ptr<const character_def> const_character_def_ptr;

	class action;
	typedef std::shared_ptr<action> action_ptr;

	typedef std::vector<uint8_t> codestream;
	typedef codestream::const_iterator codestream_iterator;

	class environment;
	typedef std::shared_ptr<environment> environment_ptr;

	class with_entry
	{
	public:
		with_entry();
		with_entry(const as_object_ptr& obj, codestream_iterator end);
	private:
		as_object_ptr obj_;
		codestream_iterator end_ip_;
	};

	typedef std::deque<with_entry> with_stack;
}
