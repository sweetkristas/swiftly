#include <boost/regex.hpp>

#include "swf_environment.hpp"

namespace swf
{
	namespace
	{
		const int max_global_registers = 256;
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

	environment::environment()
		: stack_(),
		  constant_pool_(),
		  registers_()
	{
		registers_.resize(max_global_registers);
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
		// Examples of things we can match
		// /A/B:FOO
		// /A/B/:FOO (same as above)
		// some.path.to.variable
		boost::regex re1("(/?([[:alnum:]])))+(/?:)?");
		boost::regex re2("(([[:alnum:]])\.)+");
		ASSERT_LOG(false, "XXX environment::get_variable");
	}

	void environment::set_variable(const std::string& name, const as_value_ptr& value, with_stack& wstack)
	{
		ASSERT_LOG(false, "XXX environment::set_variable");
	}
}
