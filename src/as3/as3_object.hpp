#pragma once

#include <boost/intrusive_ptr.hpp>
#include <map>
#include <memory>
#include <string>
#include "../ref_counted_ptr.hpp"

namespace avm2
{
	class as3_value;
	class as3_object;
	typedef boost::intrusive_ptr<as3_object> as3_object_ptr;

	class as3_object : public reference_counted_ptr
	{
	public:
		as3_object();
		virtual ~as3_object() {}

		virtual const char*	to_string() { return "[object Object]"; }
		virtual double to_number();
		virtual bool to_bool() { return true; }

		void builtin(const std::string& name, const as3_value& value);
	private:
		std::map<std::string, as3_value> members_;

		as3_object(const as3_object&);
	};

}
