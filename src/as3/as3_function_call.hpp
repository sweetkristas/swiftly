#pragma once

#include "as3_object.hpp"
#include "as3_value.hpp"
#include "../swf_environment.hpp"

namespace avm2
{
	class function_call
	{
	public:
		function_call();
		~function_call();
		size_t arg_count() const { return arg_count_; }
		as3_value_ptr arg(size_t n) const;
	private:
		as3_object_ptr this_;
		as3_value_ptr result_;
		//swf::environment env_;
		size_t arg_count_;
		swf::stack_iterator arg_iter_;
	};
}
