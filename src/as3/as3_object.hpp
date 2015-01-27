#pragma once

#include <map>
#include <memory>
#include <string>
#include "../swf_fwd.hpp"
#include "as3_hint.hpp"

namespace avm2
{
	class as3_value;

	class as3_object
	{
	public:
		as3_object(swf::player_ptr player);
		virtual ~as3_object() {}

		virtual const char*	to_string() { return "[object Object]"; }
		virtual double to_number();
		virtual bool to_bool() { return true; }
		virtual as3_value default_value(HintType hint=NO_HINT);

		void builtin(const std::string& name, const as3_value& value);
	private:
		swf::player_ptr player_;
		std::map<std::string, as3_value> members_;

		as3_object(const as3_object&);
	};
	typedef std::shared_ptr<as3_object> as3_object_ptr;
}
