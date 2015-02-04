#pragma once

#include "as_fwd.hpp"
#include "swf_environment.hpp"

namespace swf
{
	class function_params
	{
	public:
		function_params(const as_value_ptr& that, const environment_ptr& env, int nargs, int first_in);
		int arg_count() const { return arg_count_; }
		as_value_ptr arg(int n) const;
		const as_object_ptr& that() const { return this_; }
		environment_ptr get_environment() const { return env_; }
	private:
		as_object_ptr this_;
		as_value_ptr this_value_;
		environment_ptr env_;
		int arg_count_;
		int first_arg_bottom_index_;
	};
}
