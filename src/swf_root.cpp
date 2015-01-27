#include "swf_movie.hpp"
#include "swf_root.hpp"

namespace swf
{
	root::root(weak_player_ptr p, movie_def_ptr d)
		: player_(p),
		  def_(d),
		  frame_size_(d->get_frame_size()),
		  frame_time_(1.0f/d->get_frame_rate()),
		  current_time_(0.0f)
	{
	}

	void root::init()
	{
	}

	int root::get_current_frame() const 
	{ 
		return def_->get_current_frame();
	}

	float root::get_frame_rate() const 
	{ 
		return def_->get_frame_rate();
	}

	void root::set_frame_rate(float fr)
	{
		def_->set_frame_rate(fr);
		frame_time_ = 1.0f / def_->get_frame_rate();
	}

	int root::twip() const
	{
		return 20;
	}

	void root::advance(float delta_time)
	{
		current_time_ += delta_time;
		if(current_time_ >= frame_time_) {
			def_->set_current_frame(def_->get_current_frame() + 1);
			current_time_ -= frame_time_;
		}
	}

	void root::set_frame_count(int fc)
	{
		def_->set_frame_count(fc);
	}

	void root::display()
	{
		ASSERT_LOG(false, "root::display()");
	}
}
