#include <cstdio>
#include <limits>

#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

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
				if(d_ == std::numeric_limits<double>::quiet_NaN()
					|| d_ == std::numeric_limits<double>::signaling_NaN()) { 
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

	std::string as3_value::to_std_string()
	{
		return to_string();
	}

	as3_value operator+(const as3_value& v1, const as3_value& v2)
	{
		auto p1 = v1.to_primitive();
		auto p2 = v2.to_primitive();
		if(p1.type_ == as3_value::STRING || p2.type_ == as3_value::STRING) {
			return as3_value(p1.to_string() + p2.to_std_string());
		}
		return as3_value(p1.to_number() + p2.to_number());
	}

	as3_value as3_value::to_primitive(HintType hint) const
	{
		if(type_ == OBJECT && o_ != NULL) {
			return o_->default_value(hint);
		} else if(type_ == PROPERTY) {
			ASSERT_LOG(false, "XXX todo PROPERTY::to_primitive");
		} 
		return *this;
	}

	int32_t as3_value::to_int32()
	{
		double num = to_number();
		if(num == std::numeric_limits<double>::quiet_NaN() 
			|| num == std::numeric_limits<double>::signaling_NaN() 
			|| abs(num) == 0 
			|| abs(num) == std::numeric_limits<double>::infinity()) {
			return 0;
		}
		double pos_int = (num > 0 ? 1 : -1) * floor(abs(num));
		return int32_t(fmod(pos_int, 4294967296));
	}

	double as3_value::to_number()
	{
		switch(type_) {
			case UNDEFINED:		return std::numeric_limits<double>::infinity();
			case BOOLEAN:		return b_ ? 1 : 0;
			case NUMERIC:		return d_;
			case OBJECT:		return o_ == NULL ? 0 : o_->to_number();
			case PROPERTY: {
				ASSERT_LOG(false, "XXX todo PROPERTY::to_number");
			}
		}
		ASSERT_LOG(type_ != STRING, "FATAL: unknown type_ value: " << type_);
		// String case -- we do thing the lazy way, not the compliant way.
		double num = std::numeric_limits<double>::infinity();
		try {
			num = boost::lexical_cast<double>(s_);
		} catch(boost::bad_lexical_cast&) {
			std::cerr << "Caught a bad floating point cast from " << s_ << " assuming infinity" << std::endl;
		}
		return num;
	}
}
