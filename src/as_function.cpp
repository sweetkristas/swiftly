#include "as_array.hpp"
#include "as_function.hpp"
#include "as_function_params.hpp"
#include "swf_action.hpp"
#include "swf_player.hpp"

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

	as_value_ptr as_function_s1::operator()(const function_params& params)
	{
		environment_ptr env = params.get_environment();
		if(params.that() && params.that()->get_environment()) {
			env = params.that()->get_environment();
		}
		auto that = env->get_target();
		if(params.that()) {
			that = params.that();
		}
		auto target = target_.lock();
		if(target != nullptr) {
			env = target->get_environment();
		}
		
		environment::frame_manager frame_manager(env);

		int n = 0;
		for(auto& p : params_) {
			env->set_local(p, params.arg(n));
			if(++n >= params.arg_count()) {
				break;
			}
		}

		env->set_local("this", as_value::create(that));
		if(that) {
			env->set_local("super", as_value::create(that->get_prototype()));
		}

		return actions_->execute(target_.lock(), false);
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

	as_value_ptr as_function_s2::operator()(const function_params& params)
	{
		environment_ptr env = params.get_environment();
		if(params.that() && params.that()->get_environment()) {
			env = params.that()->get_environment();
		}
		auto that = env->get_target();
		if(params.that()) {
			that = params.that();
		}
		auto target = target_.lock();
		if(target != nullptr) {
			env = target->get_environment();
		}
		
		environment::frame_manager frame_manager(env);
		env->create_local_registers(num_regs_);

		int n = 0;
		for(auto& p : params_) {
			if(p.first == 0) {
				env->set_local(p.second, params.arg(n));
			} else {
				env->set_register(p.first, params.arg(n));
			}
			if(++n >= params.arg_count()) {
				break;
			}
		}

		// Implicit arguments
		int reg = 1;
		if(flags_ & Function2Flags::PRELOAD_THIS) {
			env->set_register(reg++, as_value::create(that));
		}
		if(!(flags_ & Function2Flags::SUPPRESS_THIS)) {
			env->set_local("this", as_value::create(that));
		}
		if((flags_ & Function2Flags::PRELOAD_ARGUMENTS) || !(flags_ & Function2Flags::SUPPRESS_ARGUMENTS)) {
			as_array_ptr args = as_array::create(env->get_player());
			for(int n = 0; n != params.arg_count(); ++n) {
				args->set_value(n, params.arg(n));
			}
			if(flags_ & Function2Flags::PRELOAD_ARGUMENTS) {
				env->set_register(reg++, as_value::create(std::static_pointer_cast<as_object>(args)));
			}
			if(!(flags_ & Function2Flags::SUPPRESS_ARGUMENTS)) {
				env->set_local("arguments", as_value::create(std::static_pointer_cast<as_object>(args)));
			}
		}

		if(flags_ & Function2Flags::PRELOAD_SUPER) {
			env->set_register(reg++, as_value::create(params.that()->get_prototype()));
		}
		if(!(flags_ & Function2Flags::SUPPRESS_SUPER)) {
			env->set_local("super", as_value::create(params.that()->get_prototype()));
		}

		if(flags_ & Function2Flags::PRELOAD_ROOT) {
			env->set_register(reg++, as_value::create(env->get_player()->get_root_movie()->get()));
		}

		if(flags_ & Function2Flags::PRELOAD_PARENT) {
			env->set_register(reg++, as_value::create(env->get_variable("_parent", with_stack())));
		}

		if(flags_ & Function2Flags::PRELOAD_GLOBAL) {
			env->set_register(reg++, as_value::create(get_global()));
		}

		return actions_->execute(target_.lock(), true);
	}
}
