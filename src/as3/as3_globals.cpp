#include "../asserts.hpp"
#include "as3_globals.hpp"
#include "../swf_player.hpp"

namespace avm2
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
}