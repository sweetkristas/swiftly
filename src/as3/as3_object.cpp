#include "as3_object.hpp"
#include "as3_value.hpp"
#include "../swf_player.hpp"

namespace avm2
{
	as3_object::as3_object(swf::player_ptr player)
		: player_(player)
	{

	}

	double as3_object::to_number()
	{
		const char* str = to_string();
		if(str) {
			return atof(str);
		}
		return 0;
	}

	void as3_object::builtin(const std::string& name, const as3_value& value)
	{
		auto it = members_.find(name);
		if(it != members_.end()) {
			std::cerr << "Replacing builtin: " << name << std::endl;
		}
		members_[name] = value;
		members_[name].set_flags(as3_value::DO_NOT_ENUM);
	}
}
