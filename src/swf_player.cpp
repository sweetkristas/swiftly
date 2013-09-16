#include "asserts.hpp"
#include "as3/as3_function_call.hpp"
#include "swf_player.hpp"

namespace swf
{
	void as3_trace_function(const avm2::function_call& fn)
	{
		ASSERT_LOG(fn.arg_count() >= 1, "Not enough arguments passed to trace function: " << fn.arg_count());		
		ASSERT_LOG(fn.arg(0) != NULL, "argument to trace is NULL pointer");
		std::string s = "undefined";
		if(!fn.arg(0)->is_undefined()) {
			s = fn.arg(0)->to_string();
		}
		std::cerr << s << std::endl;
	}

	player::player()
	{
		global_ = new avm2::as3_object(this);
		
		init();
	}

	player::~player()
	{
	}

	void player::init()
	{
		ASSERT_LOG(global_ != NULL, "player::init(): global_ is NULL");
		global_->builtin("trace", as3_trace_function);
		//global_->builtin("Object");
		//global_->builtin("Sound");
	}
}