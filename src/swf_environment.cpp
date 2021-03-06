#include <boost/regex.hpp>

#include "as_object.hpp"
#include "swf_environment.hpp"

namespace swf
{
	namespace
	{
		const int max_global_registers = 4;
	}

	with_entry::with_entry()
		: obj_(),
		  end_ip_()
	{
	}
	with_entry::with_entry(const as_object_ptr& obj, codestream_iterator end)
		: obj_(obj),
		  end_ip_(end)
	{
	}

	environment::environment(const weak_player_ptr& player)
		: player_(player),
		  stack_(),
		  constant_pool_()
	{
		global_registers_.resize(max_global_registers);
	}

	void environment::push(const as_value_ptr& value)
	{
		stack_.push_back(value);
	}

	as_value_ptr environment::pop()
	{
		auto ptr = stack_.back();
		stack_.pop_back();
		return ptr;
	}

	const std::string& environment::get_constant(int index)
	{
		ASSERT_LOG(index < static_cast<int>(constant_pool_.size()), "Index of: " << index << " is outside the constant pool bounds.");
		return constant_pool_[index];
	}

	void environment::set_constant(int index, const std::string& value)
	{
		if(index > static_cast<int>(constant_pool_.size())) {
			constant_pool_.resize(index+1);
		}
		constant_pool_[index] = value;
	}

	void environment::set_constant_pool(const std::vector<std::string>& values)
	{
		constant_pool_ = values;
	}

	void environment::clear_constants()
	{
		constant_pool_.clear();
	}

	as_value_ptr environment::get_variable(const std::string& name, with_stack& wstack)
	{
		std::vector<std::string> parts;
		if(path_parse(name, &parts)) {
		}
		// Examples of things we can match
		// /A/B:FOO
		// /A/B/:FOO (same as above)
		// some.path.to.variable
		ASSERT_LOG(false, "XXX environment::get_variable");
	}

	void environment::set_variable(const std::string& name, const as_value_ptr& value, with_stack& wstack)
	{
		ASSERT_LOG(false, "XXX environment::set_variable");
	}

	as_object_ptr environment::find_target(const as_value_ptr& value)
	{
		if(target_) {
			return target_->find_target(value);
		}
		return value->to_object();
	}

	void environment::set_member(const std::string& name, const as_value_ptr& value)
	{
		if(target_) {
			target_->set_member(name, value);
		}
	}

	as_value_ptr environment::get_member(const std::string& name)
	{
		if(target_) {
			return target_->get_member(name);
		}
		return as_value_ptr();
	}

	as_value_ptr environment::top(int n)
	{
		ASSERT_LOG(n < static_cast<int>(stack_.size()), "Tried to access member element outside stack range.");
		auto it = stack_.rbegin() + n;
		return *it;
	}

	as_value_ptr environment::bottom(int n)
	{
		ASSERT_LOG(n < static_cast<int>(stack_.size()), "Tried to access member element outside stack range.");
		auto it = stack_.begin() + n;
		return *it;
	}

	void environment::set_register(int n, const as_value_ptr& value, bool have_local_registers)
	{
		if(have_local_registers) {
			ASSERT_LOG(n >= 0 && n < static_cast<int>(local_registers_.size()), "set_register: index out of bounds: " << n);
			local_registers_[n] = value;
		} else {
			ASSERT_LOG(n >= 0 && n < static_cast<int>(global_registers_.size()), "set_register: index out of bounds: " << n);
			global_registers_[n] = value;
		}
	}

	as_value_ptr environment::get_register(int n, bool have_local_registers)
	{
		if(have_local_registers) {
			ASSERT_LOG(n >= 0 && n < static_cast<int>(local_registers_.size()), "set_register: index out of bounds: " << n);
			return local_registers_[n];
		}
		ASSERT_LOG(n >= 0 && n < static_cast<int>(global_registers_.size()), "set_register: index out of bounds: " << n);
		return global_registers_[n];
	}

	void environment::create_local_registers(int n)
	{
		local_registers_.resize(n);
	}

	void environment::set_local(const std::string& name, const as_value_ptr& value)
	{
		ASSERT_LOG(!frames_.empty(), "No local frames have been created for the parameter.");
		auto it = frames_.back().find(name);
		if(it != frames_.back().end()) {
			it->second = value->clone();
		} else {
			frames_.back()[name] = value->clone();
		}
	}
}
