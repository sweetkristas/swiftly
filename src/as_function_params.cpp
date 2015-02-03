#include "asserts.hpp"
#include "as_fwd.hpp"
#include "as_function_params.hpp"
#include "swf_player.hpp"

namespace swf
{
	function_params::function_params(const as_value_ptr& that, const environment_ptr& env, int num_args, int first_in)
		: this_(),
		  this_value_(that),
		  env_(env),
		  arg_count_(num_args),
		  first_arg_bottom_index_(first_in)
	{
		this_ = that->to_object();
	}

	as_value_ptr function_params::arg(int n) const
	{
		ASSERT_LOG(n < arg_count_, "When accessing arguments, value exceeds maximum number. " << n << " >= " << arg_count_);
		return env_->bottom(first_arg_bottom_index_ - n);
	}
}