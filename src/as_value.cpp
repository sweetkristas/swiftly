#include <cstdio>
#include <limits>

#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include "asserts.hpp"
#include "as_function_params.hpp"
#include "as_object.hpp"
#include "as_value.hpp"
#include "swf_environment.hpp"
#include "swf_player.hpp"

namespace swf
{
	as_value_ptr as_value::from_bool(bool value)
	{
		return std::make_shared<as_value>(value);
	}

	as_value::as_value(as_value_ptr get, as_value_ptr set)
		: type_(ValueType::PROPERTY),
		  b_(false),
		  d_(0),
		  o_(nullptr),
		  p_(std::make_shared<as_property>(get, set)),
		  flags_(PropertyFlags::NONE)
	{
	}

	as_value::as_value(as_native_function_type fn)
		: type_(ValueType::OBJECT),
		  b_(false),
		  d_(0),
		  flags_(PropertyFlags::NONE)
	{
		o_ = as_native_function::create(weak_player_ptr(), fn);
	}

	const char*	as_value::to_string() const
	{
		return to_std_string().c_str();
	}

	std::string as_value::to_std_string() const
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
						return s_sci;
					} 
					return s_fix;
				}
			}
			case ValueType::STRING: return s_;
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

	as_value operator+(const as_value& v1, const as_value& v2)
	{
		auto p1 = v1.to_primitive();
		auto p2 = v2.to_primitive();
		if(p1.type_ == ValueType::STRING || p2.type_ == ValueType::STRING) {
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
			case ValueType::STRING:
				break;
			case ValueType::OBJECT:			return o_;
			case ValueType::PROPERTY: {

				ASSERT_LOG(false, "XXX todo PROPERTY::to_object");
			}
		}
		return nullptr;
	}

	as_value_ptr as_value::clone()
	{
		return std::make_shared<as_value>(*this);
	}

	void as_value::set_property(const as_value_ptr& value)
	{
		ASSERT_LOG(is_property(), "Attempt to set property value on non-property.");
		p_->set(o_, value);
	}

	as_value_ptr as_value::get_property() const
	{
		ASSERT_LOG(is_property(), "Attempt to get property value on non-property.");
		return p_->get(o_);
	}

	as_value_ptr as_value::get_property(const as_value_ptr& primitive) const
	{
		ASSERT_LOG(is_property(), "Attempt to get property value on non-property.");
		return p_->get(primitive);
	}

	void as_value::set_property_target(const as_object_ptr& target)
	{
		ASSERT_LOG(is_property(), "Attempt to set property target on non-property.");
		o_ = target;
	}

	as_property::as_property(const as_value_ptr& get, const as_value_ptr& set)
	{
		get_ = std::dynamic_pointer_cast<as_function>(get->to_object());
		set_ = std::dynamic_pointer_cast<as_function>(set->to_object());
	}

	void as_property::set(const as_object_ptr& target, const as_value_ptr& value)
	{
		if(target) {
			if(set_) {
				auto env = environment::create(target->get_player());
				env->push(value);
				(*set_)(function_params(as_value::create(target), env, 1, env->get_top_index()));
			}
		} else {
			LOG_WARN("Tried to set property on null target.");
		}
	}

	as_value_ptr as_property::get(const as_object_ptr& target)
	{
		as_value_ptr value = as_value::create();
		if(target) {
			if(get_) {
				auto env = environment::create(target->get_player());
				value = (*get_)(function_params(as_value::create(target), env, 0, 0));
			}
		} else {
			LOG_WARN("Tried to set property on null target.");
		}
		return value;
	}

	as_value_ptr as_property::get(const as_value_ptr& primitive)
	{
		if(get_) {
			return (*get_)(function_params(primitive, nullptr, 0, 0));
		}
		return nullptr;
	}

	as_function_ptr as_value::to_function()
	{
		if(type_ == ValueType::OBJECT && o_ != nullptr) {
			// XXX This is a little icky -- maybe as_object should have a virtual operator().
			return std::dynamic_pointer_cast<as_function>(o_);
		}
		LOG_WARN("couldn't convert as_value to function.");
		return nullptr;
	}

	as_value_ptr as_value::find_property(const std::string& name)
	{
		switch(type_) {
			case ValueType::UNDEFINED:
			case ValueType::NULL_VALUE:
			case ValueType::PROPERTY:
				break;
			case ValueType::STRING:
				return player::get_builtin_string_method(name);
			case ValueType::BOOLEAN:
				return player::get_builtin_boolean_method(name);
			case ValueType::NUMERIC:
				return player::get_builtin_numeric_method(name);
			case ValueType::OBJECT: {
				if(o_) {
					return o_->get_member(name);
				}
				break;
			}
		}
		return nullptr;
	}
}
