#pragma once

#include "as_hint.hpp"
#include "swf_fwd.hpp"

namespace swf
{
	class function_params;
	class as_function;
	typedef std::shared_ptr<as_function> as_function_ptr;

	struct as_object_def;
	typedef std::shared_ptr<as_object_def> as_object_def_ptr;

	class as_object;
	typedef std::shared_ptr<as_object> as_object_ptr;

	class as_value;
	typedef std::shared_ptr<as_value> as_value_ptr;
}
