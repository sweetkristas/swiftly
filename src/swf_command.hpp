#pragma once

#include "asserts.hpp"
#include "swf_character.hpp"
#include "swf_fwd.hpp"
#include "swf_types.hpp"

namespace swf
{
	class command
	{
	public:
		command();
		virtual ~command();
		virtual void execute(const character_ptr& ch) = 0;
	};

	typedef std::shared_ptr<command> command_ptr;

	class set_background_color : public command
	{
	public:
		MAKE_FACTORY(set_background_color);
		void execute(const character_ptr& ch) override {
			ch->set_background_color(color_);
		}
	private:
		set_background_color(rgba color) : color_(color) {
			LOG_DEBUG("set_background_color: " << color);
		}
		rgba color_;
	};

	class place_object : public command
	{
	public:
		MAKE_FACTORY(place_object);
		void execute(const character_ptr& ch) override {
			
		}
	private:
		place_object(int id, int depth, const matrix2x3& m, const color_transform& ct) 
			: id_(id),
			  depth_(depth),
			  transform_(m),
			  color_transform_(ct)
		{
			LOG_DEBUG("place_object: " << id << ", depth: " << depth_);
		}
		int id_;
		int depth_;
		matrix2x3 transform_;
		color_transform color_transform_;
	};
}
