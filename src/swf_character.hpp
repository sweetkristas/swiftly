#pragma once

#include "as_object.hpp"
#include "swf_tags.hpp"

namespace swf
{
	class character;
	typedef std::shared_ptr<character> character_ptr;
	typedef std::weak_ptr<character> weak_character_ptr;

	class character_def;
	typedef std::shared_ptr<character_def> character_def_ptr;

	class character_def : public as_object_def
	{
	public:
		character_def();
		virtual ~character_def();

		virtual bool is_a(ASClass id) override { return id == ASClass::CHARACTER_DEF; }
	private:
	};

	class character : public as_object
	{
	public:
		character(player_ptr player, weak_character_ptr parent, int id);
		virtual ~character();

		virtual bool is_a(ASClass id) override { return id == ASClass::CHARACTER; }

		virtual bool is_tag_allowable(Tag tag) { return true; }

		void set_background_color(const rgba& color);
		const rgba& get_background_color() const { return background_color_; }

		void set_background_alpha(int a=255);
	private:		
		weak_character_ptr parent_;
		int id_;
		rgba background_color_;
	};
}
