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
		movie_ = movie::create(p, nullptr, -1, d);
	}

	void root::init()
	{
		// XXX execute init frame 0 tags here.
	}

	void root::set_frame_size(const rect& r)
	{
		frame_size_ = r;
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

	void root::update(float delta_time)
	{
		current_time_ += delta_time;
		if(current_time_ >= frame_time_) {
			movie_->update(delta_time);
			current_time_ -= frame_time_;
		}
	}

	void root::set_frame_count(int fc)
	{
		def_->set_frame_count(fc);
	}

	void root::draw() const
	{
		movie_->draw();
	}

	character_ptr root::get_character_from_id(int id)
	{
		return def_->get_character_from_id(id);
	}

	character_def_ptr root::get_movie_def() 
	{ 
		return movie_->get_definition(); 
	}
}
