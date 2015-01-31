#include <cstdio>
#include <limits>

#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include "asserts.hpp"
#include "as_value.hpp"
#include "as_object.hpp"
#include "swf_player.hpp"

namespace swf
{
	const char*	as_value::to_string() const
	{
		switch(type_) {
			case ValueType::UNDEFINED: return "undefined";
			case ValueType::BOOLEAN: return b_ ? "true" : "false";
			case ValueType::NUMERIC: {
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
			case ValueType::STRING: return s_.c_str();
			case ValueType::NULL_VALUE: return "null";
			case ValueType::OBJECT: return o_->to_string();
			case ValueType::PROPERTY:
				/*
				convert property p_ to a string.
				*/
			default: ASSERT_LOG(false, "to_string() bad type: " << static_cast<int>(type_));
		}
		return "undefined";
	}

	std::string as_value::to_std_string()
	{
		if(type_ == ValueType::STRING) {
			return s_;
		}
		return to_string();
	}

	as_value operator+(const as_value& v1, const as_value& v2)
	{
		auto p1 = v1.to_primitive();
		auto p2 = v2.to_primitive();
		if(p1.type_ == as_value::ValueType::STRING || p2.type_ == as_value::ValueType::STRING) {
			return as_value(p1.to_string() + p2.to_std_string());
		}
		return as_value(p1.to_number() + p2.to_number());
	}

	as_value as_value::to_primitive(HintType hint) const
	{
		if(type_ == ValueType::OBJECT && o_ != nullptr) {
			return o_->default_value(hint);
		} else if(type_ == ValueType::PROPERTY) {
			ASSERT_LOG(false, "XXX todo PROPERTY::to_primitive");
		} 
		return *this;
	}

	int32_t as_value::to_int32()
	{
		double num = to_number();
		if(num == std::numeric_limits<double>::quiet_NaN() 
			|| num == std::numeric_limits<double>::signaling_NaN() 
			|| abs(num) == 0 
			|| abs(num) == std::numeric_limits<double>::infinity()) {
			return 0;
		}
		double pos_int = (num > 0 ? 1 : -1) * floor(abs(num));
		return static_cast<int32_t>(fmod(pos_int, 4294967296));
	}

	int as_value::to_integer()
	{
		/// Need to double check this against the standard.
		double num = to_number();
		if(num == std::numeric_limits<double>::quiet_NaN() 
			|| num == std::numeric_limits<double>::signaling_NaN() 
			|| abs(num) == 0 
			|| abs(num) == std::numeric_limits<double>::infinity()) {
			return 0;
		}
		return static_cast<int>((num > 0 ? 1 : -1) * floor(abs(num)));
	}

	double as_value::to_number()
	{
		switch(type_) {
			case ValueType::UNDEFINED:		return std::numeric_limits<double>::infinity();
			case ValueType::BOOLEAN:		return b_ ? 1 : 0;
			case ValueType::NUMERIC:		return d_;
			case ValueType::NULL_VALUE:		return 0;
			case ValueType::OBJECT:			return o_->to_number();
			case ValueType::PROPERTY: {
				ASSERT_LOG(false, "XXX todo PROPERTY::to_number");
			}
		}
		ASSERT_LOG(type_ != ValueType::STRING, "FATAL: unknown type_ value: " << static_cast<int>(type_));
		// String case -- we do thing the lazy way, not the compliant way.
		double num = std::numeric_limits<double>::infinity();
		try {
			num = boost::lexical_cast<double>(s_);
		} catch(boost::bad_lexical_cast&) {
			std::cerr << "Caught a bad floating point cast from " << s_ << " assuming infinity" << std::endl;
		}
		return num;
	}

	bool as_value::to_boolean()
	{
		// XXX check for correctness.
		return to_integer() != 0;
	}

	as_object_ptr as_value::to_object()
	{
		switch(type_) {
			case ValueType::UNDEFINED:
			case ValueType::BOOLEAN:
			case ValueType::NUMERIC:
			case ValueType::NULL_VALUE:
				break;
			case ValueType::OBJECT:			return o_;
			case ValueType::PROPERTY: {

				ASSERT_LOG(false, "XXX todo PROPERTY::to_object");
			}
		}
		return nullptr;
	}
}
