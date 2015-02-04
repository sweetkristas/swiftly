#include <boost/lexical_cast.hpp>

#include "as_array.hpp"
#include "as_function_params.hpp"
#include "as_value.hpp"
#include "swf_player.hpp"
#include "swf_root.hpp"

namespace swf
{
	namespace
	{
		as_value_ptr get_join()
		{
			static as_value_ptr res = as_value::create(as_array::array_join);
			return res;
		}
		as_value_ptr get_concat()
		{
			static as_value_ptr res = as_value::create(as_array::array_concat);
			return res;
		}
		as_value_ptr get_slice()
		{
			static as_value_ptr res = as_value::create(as_array::array_slice);
			return res;
		}
		as_value_ptr get_unshift()
		{
			static as_value_ptr res = as_value::create(as_array::array_unshift);
			return res;
		}
		as_value_ptr get_sort()
		{
			static as_value_ptr res = as_value::create(as_array::array_sort);
			return res;
		}
		as_value_ptr get_shift()
		{
			static as_value_ptr res = as_value::create(as_array::array_shift);
			return res;
		}
		as_value_ptr get_to_string()
		{
			static as_value_ptr res = as_value::create(as_array::array_to_string);
			return res;
		}
		as_value_ptr get_push()
		{
			static as_value_ptr res = as_value::create(as_array::array_push);
			return res;
		}
		as_value_ptr get_pop()
		{
			static as_value_ptr res = as_value::create(as_array::array_pop);
			return res;
		}
		as_value_ptr get_length()
		{
			static as_value_ptr res = as_value::create(as_array::array_length);
			return res;
		}
		as_value_ptr get_splice()
		{
			static as_value_ptr res = as_value::create(as_array::array_splice);
			return res;
		}
	}

	as_array::as_array(weak_player_ptr player)
		: as_object(player)
	{
	}

	void as_array::init()
	{
		set_builtin_member("join", get_join());
		set_builtin_member("concat", get_concat());
		set_builtin_member("slice", get_slice());
		set_builtin_member("unshift", get_unshift());
		set_builtin_member("sort", get_sort());
		set_builtin_member("shift", get_shift());
		set_builtin_member("toString", get_to_string());
		set_builtin_member("push", get_push());
		set_builtin_member("pop", get_pop());
		set_builtin_member("length", get_length());
		set_builtin_member("splice", get_splice());
		set_constructor(as_value::create(array_constructor));
	}

	void as_array::sort(SortOptions opts, std::function<bool(const as_value_ptr&,const as_value_ptr&)> cmp_fn)
	{
		std::sort(ary_.begin(), ary_.end(), cmp_fn);

		if(opts & SortOptions::DESCENDING) {
			std::reverse(ary_.begin(), ary_.end());
		}
	}

	as_value_ptr as_array::array_constructor(const function_params& params)
	{
		ASSERT_LOG(params.get_environment() != nullptr, "No environment given.");
		auto ary = as_array::create(params.get_environment()->get_player());

		if(params.arg_count() == -1) {
			// special case for init array
			int cnt = params.get_environment()->pop()->to_integer();
			ary->ary_.resize(cnt);
			for(int n = 0; n != cnt; ++n) {
				ary->ary_[n] = params.get_environment()->pop();
			}
		} else if(params.arg_count() == 1) {
			int cnt = params.arg(0)->to_integer();
			ary->ary_.resize(cnt);
		} else {
			ary->ary_.resize(params.arg_count());
			for(int n = 0; n != params.arg_count(); ++n) {
				ary->ary_[n] = params.arg(n);
			}
		}

		return as_value::create(std::static_pointer_cast<as_object>(ary));
	}

	as_value_ptr as_array::array_join(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_join");
		return nullptr;
	}

	as_value_ptr as_array::array_concat(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_concat");
		return nullptr;
	}

	as_value_ptr as_array::array_slice(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_slice");
		return nullptr;
	}

	as_value_ptr as_array::array_unshift(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_unshift");
		return nullptr;
	}

	as_value_ptr as_array::array_sort(const function_params& params)
	{
		auto ary = std::dynamic_pointer_cast<as_array>(params.that());
		if(ary) {
			SortOptions opts = SortOptions::NONE;
			as_function_ptr cmp_fn = nullptr;
			if(params.arg_count() >= 1) {
				if(params.arg(0)->is_numeric()) {
					opts = static_cast<SortOptions>(params.arg(0)->to_integer());
				} else {
					cmp_fn = params.arg(0)->to_function();
				}

				if(params.arg_count() >= 2) {
					if(params.arg(1)->is_numeric()) {
						opts = static_cast<SortOptions>(params.arg(1)->to_integer());
					} else {
						cmp_fn = params.arg(1)->to_function();
					}
				}
			}
			if(cmp_fn == nullptr) {
				ary->sort(opts, [](const as_value_ptr& lhs, const as_value_ptr& rhs){ 
					return lhs->to_number() > rhs->to_number();
				});
			} else {
				auto this_pointer = as_value::create(ary->shared_from_this());
				ary->sort(opts, [&](const as_value_ptr& lhs, const as_value_ptr& rhs){ 
					auto env = ary->get_player()->get_root_movie()->get_environment();
					env->push(lhs);
					env->push(rhs);
					auto ret = (*cmp_fn)(function_params(this_pointer, env, 2, env->get_top_index()));
					return ret->to_integer() > 0;
				});
			}
		}
		return nullptr;
	}

	as_value_ptr as_array::array_shift(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_shift");
		return nullptr;
	}

	as_value_ptr as_array::array_to_string(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_to_string");
		return nullptr;
	}

	as_value_ptr as_array::array_push(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_push");
		return nullptr;
	}

	as_value_ptr as_array::array_pop(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_pop");
		return nullptr;
	}

	as_value_ptr as_array::array_length(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_length");
		return nullptr;
	}

	as_value_ptr as_array::array_splice(const function_params& params)
	{
		ASSERT_LOG(false, "XXX array_splice");
		return nullptr;
	}

	void as_array::set_value(int n, const as_value_ptr& value)
	{
		if(static_cast<int>(ary_.size()) < n) {
			ary_.resize(n+1);
		}
		ary_[n] = value->clone();
	}

	as_value_ptr as_array::get_value(int n) const
	{
		//ASSERT_LOG(n < static_cast<int>(ary_.size()), "array index out of bounds.");
		return n < static_cast<int>(ary_.size()) ? ary_[n] : nullptr;
	}

	as_value_ptr as_array::get_member(const std::string& name)
	{
		try {
			int n = boost::lexical_cast<int>(name);
			if(n >= 0 && n < size()) {
				return ary_[n];
			}
		} catch(boost::bad_lexical_cast&) {
			// ...
		}
		return as_object::get_member(name);
	}

	void as_array::set_member(const std::string& name, const as_value_ptr& value)
	{
		try {
			int n = boost::lexical_cast<int>(name);
			if(n >= 0) {
				if(n >= size()) {
					ary_.resize(n + 1);
				}
				ary_[n] = value;
			}
		} catch(boost::bad_lexical_cast&) {
			as_object::set_member(name, value);
		}
	}

}
