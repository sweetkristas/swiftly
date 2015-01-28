#pragma once

#include <sstream>

#include "as3_object.hpp"

namespace avm2
{
	class function_call;

	typedef void(*as_native_function_type)(const function_call& fn);

	class as3_function : public as3_object
	{
	public:
		as3_function(swf::player_ptr player) : as3_object(player) {
		}
		virtual ~as3_function() {
		}
		virtual void operator()(const function_call& fn) = 0;

		virtual const char*	type_of() { return "function"; }
		virtual const char*	to_string() {
			std::stringstream ss;
			ss << static_cast<void*>(this);
			return ss.str().c_str();
		}
	private:
	};
	typedef std::shared_ptr<as3_function> as3_function_ptr;

	class as3_native_function : public as3_function
	{
	public:
		as3_native_function(swf::player_ptr player, as_native_function_type fn) : as3_function(player) {
		}
		virtual ~as3_native_function() {
		}
		void operator()(const function_call& fn) {
			if(fn_) {
				fn_(fn);
			}
		}
	private:
		as_native_function_type fn_;
	};
	typedef std::shared_ptr<as3_native_function> as3_native_function_ptr;
}
