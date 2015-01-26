#include "swf_movie.hpp"

namespace swf
{
	movie::movie(int frame_count)
		: current_frame_(0),
		  max_frames_(frame_count)
	{
	}

	movie::~movie()
	{
	}

	void movie::set_frame_count(int frame_count)
	{
		max_frames_ = frame_count;
	}
}
