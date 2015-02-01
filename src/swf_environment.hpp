#pragma once

#include <deque>

#include "as_value.hpp"

namespace swf
{
	typedef std::deque<as_value_ptr> stack;
	typedef std::deque<as_value_ptr>::iterator stack_iterator;

	class environment
	{
	public:
		MAKE_FACTORY(environment);
		void push(const as_value_ptr& value);
		as_value_ptr pop();

		void clear_constants();
		const std::string& get_constant(int index);
		void set_constant(int index, const std::string& value);
		void set_constant_pool(const std::vector<std::string>& values);

		as_value_ptr get_variable(const std::string& name, with_stack& wstack);
		void set_variable(const std::string& name, const as_value_ptr& value, with_stack& wstack);

		void set_member(const std::string& name, const as_value_ptr& value);
		as_value_ptr get_member(const std::string& name);

		as_object_ptr find_target(const as_value_ptr& value);
	private:
		explicit environment(const weak_player_ptr& player);
		void init() {}
		weak_player_ptr player_;
		stack stack_;
		std::vector<std::string> constant_pool_;
		std::vector<as_value_ptr> registers_;

		as_object_ptr target_;
	};
}
