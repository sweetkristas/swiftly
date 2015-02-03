#pragma once

#include <memory>
#include <string>
#include "as_function.hpp"
#include "as_fwd.hpp"

namespace swf
{
	class function_params;

	class as_property
	{
	public:
		as_property(const as_value_ptr& get, const as_value_ptr& set);
		void set(const as_object_ptr& target, const as_value_ptr& value);
		as_value_ptr get(const as_object_ptr& target);
		as_value_ptr get(const as_value_ptr& primitive);
	private:
		as_function_ptr get_;
		as_function_ptr set_;
	};
	typedef std::shared_ptr<as_property> as_property_ptr;

	enum class PropertyFlags {
		NONE			= 0x00,
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

	class as_value
	{
	public:
		MAKE_FACTORY(as_value);

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
		explicit as_value(as_value_ptr get, as_value_ptr set);
		explicit as_value(as_native_function_type fn);
		virtual ~as_value() {}

		static as_value_ptr from_bool(bool value=true);

		bool is_bool() const { return type_ == ValueType::BOOLEAN; }
		bool is_numeric() const { return type_ == ValueType::NUMERIC; }
		bool is_string() const { return type_ == ValueType::STRING; }
		bool is_object() const { return type_ == ValueType::OBJECT; }
		bool is_undefined() const { return type_ == ValueType::UNDEFINED; }
		bool is_property() const { return type_ == ValueType::PROPERTY; }

		double to_number();
		int32_t to_int32();
		uint32_t to_uint32();
		bool to_boolean();
		int to_integer();
		std::string to_std_string() const;
		as_object_ptr to_object();
		as_function_ptr to_function();

		as_value to_primitive(HintType hint=HintType::NO_HINT) const;

		const char*	to_string() const;
		
		void set_flag(PropertyFlags f) { flags_ = static_cast<PropertyFlags>(static_cast<int>(f)|static_cast<int>(flags_)); }
		void clear_all_flags() { flags_ = static_cast<PropertyFlags>(0); }
		void clear_flag(PropertyFlags f) { flags_ = static_cast<PropertyFlags>(static_cast<int>(flags_) & ~(static_cast<int>(f))); }
		bool is_readonly() const { 
			return (static_cast<int>(flags_) & static_cast<int>(PropertyFlags::READ_ONLY)) == static_cast<int>(PropertyFlags::READ_ONLY); 
		}
		bool do_not_enum() const { 
			return (static_cast<int>(flags_) & static_cast<int>(PropertyFlags::DO_NOT_ENUM)) == static_cast<int>(PropertyFlags::DO_NOT_ENUM); 
		}
		bool do_not_delete() const { 
			return (static_cast<int>(flags_) & static_cast<int>(PropertyFlags::DO_NOT_DELETE)) == static_cast<int>(PropertyFlags::DO_NOT_DELETE); 
		}

		void set_property(const as_value_ptr& value);
		as_value_ptr get_property(const as_value_ptr& primitive) const;
		as_value_ptr get_property() const;
		void set_property_target(const as_object_ptr& target);
		as_value_ptr find_property(const std::string& name);

		friend as_value operator+(const as_value& v1, const as_value& v2);

		as_value_ptr clone();
	private:
		void init() {}
		ValueType type_;

		bool b_;
		double d_;
		std::string s_;
		as_object_ptr o_;
		// If this is a property then o_ is the property target.
		as_property_ptr p_;

		PropertyFlags flags_;
	};
	typedef std::shared_ptr<as_value> as_value_ptr;	
}
