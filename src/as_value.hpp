#pragma once

#include <memory>
#include <string>
#include "as_function.hpp"
#include "as_fwd.hpp"

namespace swf
{
	class as_value;
	class function_call;

	class as_property
	{
	public:
		as_property(const as_value& get, const as_value& set) {
		}
		virtual ~as_property() {}
	private:
		as_function_ptr get_;
		as_function_ptr set_;
	};
	typedef std::shared_ptr<as_property> as_property_ptr;

	class as_value
	{
	public:
		MAKE_FACTORY(as_value);

		enum class PropertyValue {
			DO_NOT_ENUM		= 0x01,
			DO_NOT_DELETE	= 0x02,
			READ_ONLY		= 0x04
		};
		enum class ValueType {
			UNDEFINED,
			BOOLEAN,
			NUMERIC,
			STRING,
			OBJECT,
			NULL_VALUE,
			PROPERTY,
		};

		as_value() : type_(ValueType::UNDEFINED) {
		}
		explicit as_value(bool b) : type_(ValueType::BOOLEAN), b_(b) {
		}
		explicit as_value(float f) : type_(ValueType::NUMERIC), d_(f) {
		}
		explicit as_value(double d) : type_(ValueType::NUMERIC), d_(d) {
		}
		explicit as_value(int d) : type_(ValueType::NUMERIC), d_(d) {
		}
		explicit as_value(const std::string& s) : type_(ValueType::STRING), s_(s) {
		}
		explicit as_value(const char* s) : type_(ValueType::STRING), s_(s) {
		}
		explicit as_value(as_object_ptr o) : type_(o == nullptr ? ValueType::NULL_VALUE : ValueType::OBJECT), o_(o) {
		}
		explicit as_value(as_native_function_type fn) : type_(ValueType::OBJECT) {
			o_.reset(new as_native_function(NULL, fn));
		}
		virtual ~as_value() {}

		bool is_bool() const { return type_ == ValueType::BOOLEAN; }
		bool is_numeric() const { return type_ == ValueType::NUMERIC; }
		bool is_string() const { return type_ == ValueType::STRING; }
		bool is_object() const { return type_ == ValueType::OBJECT; }
		bool is_undefined() const { return type_ == ValueType::UNDEFINED; }

		double to_number();
		int32_t to_int32();
		uint32_t to_uint32();
		bool to_boolean();
		int to_integer();
		std::string to_std_string();
		as_object_ptr to_object();

		as_value to_primitive(HintType hint=HintType::NO_HINT) const;

		const char*	to_string() const;
		
		void set_flags(uint32_t f) { flags_ = f; }

		friend as_value operator+(const as_value& v1, const as_value& v2);
	private:
		void init() {}
		ValueType type_;

		bool b_;
		double d_;
		std::string s_;
		as_object_ptr o_;
		as_property_ptr p_;

		uint32_t flags_;
	};
	typedef std::shared_ptr<as_value> as_value_ptr;	
}
