#pragma once

#include "swf_fwd.hpp"
#include "swf_character.hpp"

namespace swf
{
	class root
	{
	public:
		MAKE_FACTORY(root);

		rect get_frame_size() const { return frame_size_; }
		void set_frame_size(const rect& r);

		int get_current_frame() const;

		float get_frame_rate() const;
		void set_frame_rate(float fr);

		int twip() const;

		void update(float delta_time);
		void draw() const;

		void set_frame_count(int fc);

		character_def_ptr get_movie_def();

		character_ptr get_character_from_id(int frame, int id);

		character_ptr get_named_character(const std::string& name);

		void export_resource(const std::string& name, const character_def_ptr& def);

		environment_ptr get_environment();

		as_object_ptr get() const { return movie_; }
	private:
		explicit root(weak_player_ptr p, movie_def_ptr d);
		// init() is called automatically when instantiating via swf::root::create(...)
		void init();

		weak_player_ptr player_;
		character_ptr movie_;
		movie_def_ptr def_;

		rect frame_size_;
		float frame_time_;

		float current_time_;
	};

	typedef std::shared_ptr<root> root_ptr;
}
