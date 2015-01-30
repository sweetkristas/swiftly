#pragma once

#include <deque>

#include "as_value.hpp"

namespace swf
{
	typedef std::deque<as_value_ptr> stack;
	typedef std::deque<as_value_ptr>::iterator stack_iterator;

	class with_entry
	{
	public:
		with_entry();
		with_entry(const as_object_ptr& obj, codestream_iterator end);
	private:
		as_object_ptr obj_;
		codestream_iterator end_ip_;
	};

	typedef std::deque<with_entry> with_stack;

	class environment
	{
	public:
		environment();
		void push(const as_value_ptr& value);
		as_value_ptr pop();

		void clear_constants();
		const std::string& get_constant(int index);
		void set_constant(int index, const std::string& value);
	private:
		stack stack_;
		with_stack with_stack_;
		std::vector<std::string> constant_pool_;
		std::vector<as_value_ptr> registers_;
	};
}
