#pragma once

#include "as_object.hpp"

namespace swf
{
	class character_def;
	typedef std::shared_ptr<character_def> character_def_ptr;
	typedef std::weak_ptr<character_def> character_def_weak_ptr;
	class character;
	typedef std::shared_ptr<character> character_ptr;
	typedef std::weak_ptr<character> character_weak_ptr;

	class character_def : public as_object_def
	{
	public:
		character_def();
		virtual ~character_def();

		static character_ptr create_instance(character_weak_ptr parent, int id);

		bool is(ASClass id) override { return id == ASClass::CHARACTER_DEF; }
	private:
	};

	class character : public as_object_def
	{
	public:
		// parent/id/etc
		character(player_ptr player, std::weak_ptr<character_def> def, character_weak_ptr parent, int id);

		bool is(ASClass id) override { return id == ASClass::CHARACTER; }
	private:
		player_ptr player_;
		character_weak_ptr parent_;
		character_def_weak_ptr def_;
		int id_;
	};
}
