#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(push) 
#pragma warning(disable:4996) 
#endif

#include <boost/algorithm/string.hpp>

#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(pop) 
#endif 

#include "as_object.hpp"
#include "as_value.hpp"

namespace swf
{

	bool path_parse(const std::string& path, std::vector<std::string>* parts)
	{
		boost::split(*parts, path, boost::is_any_of("/:"), boost::token_compress_on);
		if(!parts->empty()) {
			parts->erase(std::remove_if(parts->begin(), parts->end(), [](const std::string& s) { return s.empty(); }));
			return true;
		}
		boost::split(*parts, path, boost::is_any_of("."));
		if(!parts->empty()) {
			parts->erase(std::remove_if(parts->begin(), parts->end(), [](const std::string& s) { return s.empty(); }));
			return true;
		}
		return false;
	}

	double as_object::to_number()
	{
		const char* str = to_string();
		if(str) {
			return atof(str);
		}
		return 0;
	}

	as_value as_object::default_value(HintType hint)
	{
		if(hint == HintType::NO_HINT || hint == HintType::HINT_NUMBER) {
			return as_value(to_number());
		}
		ASSERT_LOG(hint != HintType::HINT_STRING, "FATAL: hint value is unknown: " << static_cast<int>(hint));
		// string hint
		return as_value(to_string());
	}

	as_value_ptr as_object::get_member(const std::string& name)
	{
		ASSERT_LOG(false, "XXX as_object::get_member");
	}

	void as_object::set_member(const std::string& name, const as_value_ptr& value)
	{
		ASSERT_LOG(false, "XXX as_object::set_member");
	}

	as_object_ptr as_object::find_target(const as_value_ptr& value)
	{
		if(!value->is_string()) {
			return value->to_object();
		}

		std::vector<std::string> parts;
		if(path_parse(value->to_std_string(), &parts)) {
			auto val = get_member(parts.front())->to_object();
			if(val) {				
				val->find_target(parts.begin()+1, parts.end());
			}
		}
		return value->to_object();
	}

	as_object_ptr as_object::find_target(std::vector<std::string>::const_iterator start, std::vector<std::string>::const_iterator end)
	{
		if(start == end) {
			return nullptr;
		}
		auto val = get_member(*start);
		return val ? val->to_object() : nullptr;
	}
}
