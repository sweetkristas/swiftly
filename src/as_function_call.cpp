#include "asserts.hpp"
#include "as_fwd.hpp"
#include "as_function_call.hpp"
#include "swf_player.hpp"

namespace swf
{
	as_value_ptr function_call::arg(size_t n) const
	{
		return *(arg_iter_+n);
	}
}