#include <cstdio>

#include "../asserts.hpp"
#include "as3_value.hpp"

namespace avm2
{
	const char*	as3_value::to_string() const
	{
		switch(type_) {
			case UNDEFINED: return "undefined";
			case BOOLEAN: return b_ ? "true" : "false";
			case NUMERIC: {
				if(_isnan(d_)) { 
					return "NaN";
				} else {
					char buff[50];
					_snprintf(buff, 50, "%.14g", d_);
					return buff;
				}
			}
			case STRING: return s_.c_str();
			case OBJECT: return o_ == NULL ? "null" : o_->to_string();
			case PROPERTY:
				/*
				convert property p_ to a string.
				*/
			default: ASSERT_LOG(false, "to_string() bad type: " << type_);
		}
		return "undefined";
	}
}
