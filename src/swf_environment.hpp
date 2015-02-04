#pragma once

#include <deque>

#include "as_value.hpp"

namespace swf
{
	typedef std::deque<as_value_ptr> stack;
	typedef std::deque<as_value_ptr>::iterator stack_iterator;

	typedef std::map<std::string, as_value_ptr> frame_type;

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

		void create_local_registers(int n);
		void set_register(int n, const as_value_ptr& value, bool have_local_registers=true);
		as_value_ptr get_register(int n, bool have_local_registers);

		struct frame_manager 
		{
			frame_manager(environment_ptr env) : env_(env) {
				env_->frames_.emplace_back(frame_type());
			}
			~frame_manager() {
				env_->frames_.pop_back();
			}
		private:
			environment_ptr env_;
		};
		void set_local(const std::string& name, const as_value_ptr& value);

		as_object_ptr find_target(const as_value_ptr& value);
		as_object_ptr get_target() const { return target_; }
		void set_target(const as_object_ptr& target) { target_ = target; }

		as_value_ptr top(int n);
		as_value_ptr bottom(int n);
		int get_top_index() const { return stack_.size() - 1; };
		int size() const { return stack_.size(); }

		player_ptr get_player() { return player_.lock(); }
	private:
		explicit environment(const weak_player_ptr& player);
		void init() {}
		weak_player_ptr player_;
		stack stack_;
		std::vector<std::string> constant_pool_;
		std::vector<as_value_ptr> local_registers_;
		std::vector<as_value_ptr> global_registers_;
		std::deque<frame_type> frames_;

		as_object_ptr target_;
	};
}
