#pragma once

#include <sstream>

#include "as_object.hpp"

namespace swf
{
	class function_call;

	typedef void(*as_native_function_type)(const function_call& fn);

	class as_function : public as_object
	{
	public:
		as_function(swf::player_ptr player) : as_object(player) {
		}
		virtual ~as_function() {
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
	typedef std::shared_ptr<as_function> as_function_ptr;

	class as_native_function : public as_function
	{
	public:
		as_native_function(swf::player_ptr player, as_native_function_type fn) : as_function(player) {
		}
		virtual ~as_native_function() {
		}
		void operator()(const function_call& fn) {
			if(fn_) {
				fn_(fn);
			}
		}
	private:
		as_native_function_type fn_;
	};
	typedef std::shared_ptr<as_native_function> as_native_function_ptr;
}
