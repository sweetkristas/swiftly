#include "as_function.hpp"
#include "swf_action.hpp"

namespace swf
{
	as_function_s2::as_function_s2(weak_player_ptr player, 
		int num_regs, 
		Function2Flags flags, 
		const std::vector<std::pair<int, std::string>>& params, 
		const code_block& codes, 
		const with_stack& wstack)
		: as_function(player),
		  flags_(flags),
		  //params_(params),
		  codes_(codes)
	{
		ASSERT_LOG(num_regs >= 0 && num_regs < 256, "Number of registers exceeds limits 0 <= " << num_regs << " < 256.");
		registers_.resize(num_regs);
	}

	void as_function_s2::operator()(const function_call& fn)
	{
		auto a = action::create(codes_);
		a->execute(target_);
	}
}
