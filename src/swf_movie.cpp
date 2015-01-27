#include <algorithm>
#include "swf_movie.hpp"

namespace swf
{
	movie_def::movie_def()
		: current_frame_(0),
		  max_frames_(0)
	{
	}

	movie_def::~movie_def()
	{
	}

	void movie_def::set_frame_count(int frame_count)
	{
		max_frames_ = frame_count;
		characters_.resize(frame_count);
		commands_.resize(frame_count);
	}

	void movie_def::set_frame_rate(float fr) 
	{ 
		frame_rate_ = std::min(120.0f, std::max(1.0f, fr)); 
	}

	void movie_def::add_character(int id, const character_def_ptr& ch)
	{
		ASSERT_LOG(get_current_frame() < characters_.size(), "Current frame exceeds the number of frames defined.");
		characters_[get_current_frame()][id] = ch;
	}

	void movie_def::show_frame()
	{
		++current_frame_;
	}

	void movie_def::add_command(const command_ptr& cmd)
	{
		ASSERT_LOG(get_current_frame() < commands_.size(), "Current frame exceeds the number of frames defined.");
		commands_[get_current_frame()].emplace_back(cmd);
	}
}
