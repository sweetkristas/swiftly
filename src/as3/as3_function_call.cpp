#include "../asserts.hpp"
#include "as3_function_call.hpp"
#include "../swf_player.hpp"

namespace avm2
{
	as3_value_ptr function_call::arg(size_t n) const
	{
		return *(arg_iter_+n);
	}
}