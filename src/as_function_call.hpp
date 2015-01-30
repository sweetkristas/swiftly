#pragma once

#include "as_fwd.hpp"
#include "swf_environment.hpp"

namespace swf
{
	class function_call
	{
	public:
		function_call();
		~function_call();
		size_t arg_count() const { return arg_count_; }
		as_value_ptr arg(size_t n) const;
	private:
		as_object_ptr this_;
		as_value_ptr result_;
		//swf::environment env_;
		size_t arg_count_;
		swf::stack_iterator arg_iter_;
	};
}
