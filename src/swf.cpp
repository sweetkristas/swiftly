#include "asserts.hpp"
#include "swf.hpp"

namespace swf
{
	swf::swf()
	{}

	swf::~swf()
	{}

	void swf::set_version(unsigned vers)
	{
		version_ = vers;
	}

	void swf::set_frame_size(rect frame_size)
	{
		frame_size_ = frame_size;
	}

	void swf::set_frame_rate(fixed_point frame_rate)
	{
		frame_rate_ = frame_rate.to_double();
	}

	void swf::set_frame_count(unsigned frame_count)
	{
		frames_.resize(frame_count);
	}

	void swf::set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network)
	{
		use_direct_blit_ = use_direct_blit;
		use_gpu_ = use_gpu;
		use_as3_ = use_as3;
		use_network_ = use_network;
		ASSERT_LOG(use_as3, "ActionScript 1.0&2.0 not supported.");
	}

	void swf::set_scene_and_frame_info(const scene_info& scene, const frame_label_info& frame_label)
	{
		scene_info_ = scene;
		frame_label_ = frame_label;
	}

	void swf::draw() const
	{
		for(auto d : display_list_) {
			d.second->draw();
		}
	}

	displayable::displayable(character_ptr cp)
		: character_(cp), clip_depth_(0), ratio_(1.0f)
	{
	}

	displayable::~displayable()
	{
	}

	void displayable::draw() const
	{
		ASSERT_LOG(character_ != NULL, "No character to draw");
		character_->draw();
	}
}
