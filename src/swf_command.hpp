#pragma once

#include "swf_action.hpp"
#include "swf_fwd.hpp"
#include "swf_params.hpp"

namespace swf
{
	struct command
	{
		command();
		virtual ~command();
		virtual void execute(const character_ptr& ch) = 0;
		virtual void init() {}
	};

	typedef std::shared_ptr<command> command_ptr;

	class set_background_color : public command
	{
	public:
		MAKE_FACTORY(set_background_color);
		void execute(const character_ptr& ch);
	private:
		set_background_color(rgba color);
		set_background_color(rgb color);
		rgba color_;
	};

	class place_object : public command
	{
	public:
		MAKE_FACTORY(place_object);
		void execute(const character_ptr& ch) override;
	private:
		place_object(int id, int depth, const matrix2x3& m, bool has_color_transform, const color_transform& ct);
		placement_params pp_;
	};

	class place_object2 : public command
	{
	public:
		enum class Operation {
			PLACE,
			MOVE,
			REPLACE,
		};

		MAKE_FACTORY(place_object2);
		void execute(const character_ptr& ch);
	private:
		place_object2(Operation op, const placement_params& pp);
		Operation op_;
		placement_params pp_;
	};

	class remove_object2 : public command
	{
	public:
		MAKE_FACTORY(remove_object2);
		void execute(const character_ptr& ch);
	private:
		remove_object2(int depth);
		int depth_;
	};

	class do_action : public command
	{
	public:
		MAKE_FACTORY(do_action);
		void execute(const character_ptr& ch);
	private:
		explicit do_action(const action_ptr& actions);
		action_ptr actions_;
	};
	
}
