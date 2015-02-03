#include "as_function.hpp"
#include "swf_action.hpp"

namespace swf
{
	as_function_s1::as_function_s1(weak_player_ptr player, 
		const std::vector<std::string>& params, 
		const code_block& codes,
		const with_stack& wstack) 
		: as_function(player),
		  params_(params),
		  actions_(action::create(codes))
	{
	}

	as_value_ptr as_function_s1::operator()(const function_params& fn)
	{
		// XXX use fn
		return actions_->execute(target_);
	}

	as_function_s2::as_function_s2(weak_player_ptr player, 
		int num_regs, 
		Function2Flags flags, 
		const std::vector<std::pair<int, std::string>>& params, 
		const code_block& codes, 
		const with_stack& wstack)
		: as_function(player),
		  params_(params),
		  num_regs_(num_regs),
		  flags_(flags),
		  actions_(action::create(codes))
	{
	}

	as_value_ptr as_function_s2::operator()(const function_params& fn)
	{
		// XXX use fn
		return actions_->execute(target_);
	}
}
