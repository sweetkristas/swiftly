#pragma once

#include <sstream>

#include "as_object.hpp"
#include "swf_code.hpp"

namespace swf
{
	class function_call;

	typedef void(*as_native_function_type)(const function_call& fn);

	class as_function : public as_object
	{
	public:
		as_function(weak_player_ptr player) : as_object(player) {
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
		virtual bool is_a(ASClass id) override  {
			return id == ASClass::FUNCTION ? true : as_object::is_a(id);
		}		
		virtual bool has_own_registers() const { return false; }
	private:
	};
	typedef std::shared_ptr<as_function> as_function_ptr;

	class as_native_function : public as_function
	{
	public:
		as_native_function(weak_player_ptr player, as_native_function_type fn) 
			: as_function(player) 
		{
		}
		void operator()(const function_call& fn) {
			if(fn_) {
				fn_(fn);
			}
		}
		virtual bool is_a(ASClass id) override  {
			return id == ASClass::C_FUNCTION ? true : as_function::is_a(id);
		}		
	private:
		as_native_function_type fn_;
	};
	typedef std::shared_ptr<as_native_function> as_native_function_ptr;

	enum class Function2Flags {
		PRELOAD_PARENT		= 0x80,
		PRELOAD_ROOT		= 0x40,
		PRELOAD_SUPER		= 0x20,
		SUPPRESS_SUPER		= 0x10,
		SUPPRESS_ARGUMENTS	= 0x08,
		PRELOAD_ARGUMENTS	= 0x04,
		SUPPRESS_THIS		= 0x02,
		PRELOAD_THIS		= 0x01,
		PRELOAD_GLOBAL		= 0x0100,
	};

	inline bool operator|(Function2Flags f1, Function2Flags f2) {
		return (static_cast<unsigned>(f1) | static_cast<unsigned>(f2)) != 0;
	}

	class as_function_s1 : public as_function
	{
	public:
		MAKE_FACTORY(as_function_s1);
		void operator()(const function_call& fn);
		virtual bool is_a(ASClass id) override  {
			return id == ASClass::S_FUNCTION ? true : as_function::is_a(id);
		}
	private:
		explicit as_function_s1(weak_player_ptr player, 
			const std::vector<std::string>& params, 
			const code_block& codes,
			const with_stack& wstack);
		std::vector<std::string> params_;
		action_ptr actions_;
		as_object_ptr target_;
	};

	class as_function_s2 : public as_function
	{
	public:
		MAKE_FACTORY(as_function_s2);
		void operator()(const function_call& fn);
		virtual bool is_a(ASClass id) override  {
			return id == ASClass::S_FUNCTION ? true : as_function::is_a(id);
		}
		bool has_own_registers() const { return true; }
	private:
		explicit as_function_s2(weak_player_ptr player, 
			int num_regs, 
			Function2Flags flags, 
			const std::vector<std::pair<int, std::string>>& params, 
			const code_block& codes, 
			const with_stack& wstack); 
		std::vector<std::pair<int, std::string>> params_;
		int num_regs_;
		Function2Flags flags_;
		action_ptr actions_;
		as_object_ptr target_;
	};
}
