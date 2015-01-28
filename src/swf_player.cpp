#include "asserts.hpp"
#include "as_object.hpp"
#include "swf_movie.hpp"
#include "swf_player.hpp"
#include "swf_reader.hpp"

namespace swf
{
	player::player()
		: use_direct_blit_(false),
		  use_gpu_(false),
		  use_as3_(false),
		  use_network_(false),
		  transparent_background_(false),
		  realtime_framerate_(false),
		  version_(0)
	{
	}

	void player::init()
	{
		global_ = as_object::create(shared_from_this());
		ASSERT_LOG(global_ != NULL, "player::init(): global_ is NULL");
		//global_->builtin("trace", avm2::as3_value(avm2::as3_trace_function));
		//global_->builtin("Object");
		//global_->builtin("Sound");
	}

	root_ptr player::load_file(const std::string& filename)
	{
		auto m = movie_def::create();
		root_ = root::create(shared_from_this(), m);
		reader swf_reader(filename, shared_from_this());
		return root_;
	}
	
	void player::set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network)
	{
		use_direct_blit_ = use_direct_blit;
		use_gpu_ = use_gpu;
		use_as3_ = use_as3;
		use_network_ = use_network;
	}
}