#include <cstdio>

#include <sstream>
#include <iomanip>

#include "../asserts.hpp"
#include "as3_value.hpp"
#include "../swf_player.hpp"

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
					std::stringstream ss_fixed;
					ss_fixed << std::setprecision(14) << std::ios::fixed << d_;
					std::string s_fix = ss_fixed.str();
					std::stringstream ss_sci;
					ss_fixed << std::setprecision(14) << std::ios::scientific << d_;
					std::string s_sci = ss_sci.str();
					if(s_sci.length() < s_fix.length()) {
						return s_sci.c_str();
					} 
					return s_fix.c_str();
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
