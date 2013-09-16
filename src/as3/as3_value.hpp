#pragma once

#include <memory>
#include <string>
#include "../ref_counted_ptr.hpp"
#include "as3_function_call.hpp"
#include "as3_function.hpp"

namespace avm2
{
	class as3_value;
	class as3_object;
	class as3_function;

	typedef void(*as_native_function_type)(const function_call& fn);

	class as3_property : public reference_counted_ptr
	{
	public:
		as3_property(const as3_value& get, const as3_value& set) {
		}
		virtual ~as3_property() {}
	private:
		std::unique_ptr<as3_function> get_;
		std::unique_ptr<as3_function> set_;
	};

	class as3_value
	{
	public:
		enum {
			DO_NOT_ENUM		= 0x01,
			DO_NOT_DELETE	= 0x02,
			READ_ONLY		= 0x04
		};

		as3_value() : type_(UNDEFINED) {
		}
		as3_value(bool b) : type_(BOOLEAN), b_(b) {
		}
		as3_value(float f) : type_(NUMERIC), d_(f) {
		}
		as3_value(double d) : type_(NUMERIC), d_(d) {
		}
		as3_value(int d) : type_(NUMERIC), d_(d) {
		}
		as3_value(const std::string& s) : type_(STRING), s_(s) {
		}
		as3_value(as3_object* o) : type_(OBJECT), o_(std::unique_ptr<as3_object>(o)) {
		}
		as3_value(as_native_function_type fn) : type_(OBJECT) {
			o_.reset(new as3_native_function(NULL, fn));
		}

		bool is_bool() const { return type_ == BOOLEAN; }
		bool is_numeric() const { return type_ == NUMERIC; }
		bool is_string() const { return type_ == STRING; }
		bool is_object() const { return type_ == OBJECT; }
		bool is_undefined() const { return type_ == UNDEFINED; }

		const char*	to_string() const;


		void set_flags(uint32_t f) { flags_ = f; }

		virtual ~as3_value() {}
	private:
		enum ValueType {
			UNDEFINED,
			BOOLEAN,
			NUMERIC,
			STRING,
			OBJECT,
			PROPERTY,
		};
		ValueType type_;

		bool b_;
		double d_;
		std::string s_;
		std::unique_ptr<as3_object> o_;
		std::unique_ptr<as3_property> p_;

		uint32_t flags_;
	};
	typedef std::shared_ptr<as3_value> as3_value_ptr;
}
