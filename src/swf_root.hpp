#pragma once

#include "swf_fwd.hpp"

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

		void advance(float delta_time);
		void display();

		void set_frame_count(int fc);

		movie_def_ptr get_movie_def() { return def_; }
	private:
		explicit root(weak_player_ptr p, movie_def_ptr d);
		// init() is called automatically when instantiating via swf::root::create(...)
		void init();

		weak_player_ptr player_;
		movie_def_ptr def_;

		rect frame_size_;
		float frame_time_;

		float current_time_;
	};

	typedef std::shared_ptr<root> root_ptr;
}
