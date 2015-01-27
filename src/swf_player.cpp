#include "asserts.hpp"
#include "as_object.hpp"
#include "swf_movie.hpp"
#include "swf_player.hpp"
#include "swf_reader.hpp"

namespace swf
{
	player::player()
		: transparent_background_(false),
		  realtime_framerate_(false),
		  version_(0)
	{
		global_ = as_object::create(shared_from_this());
	}

	void player::init()
	{
		ASSERT_LOG(global_ != NULL, "player::init(): global_ is NULL");
		//global_->builtin("trace", avm2::as3_value(avm2::as3_trace_function));
		//global_->builtin("Object");
		//global_->builtin("Sound");
	}

	root_ptr player::load_file(const std::string& filename)
	{
		auto m = movie_def::create();
		reader swf_reader(filename, shared_from_this());
		root_ = root::create(shared_from_this(), m);
		return root_;
	}
}