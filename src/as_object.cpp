#include "as_object.hpp"
#include "as_value.hpp"

namespace swf
{
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
}
