#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(push) 
#pragma warning(disable:4996) 
#endif

#include <boost/algorithm/string.hpp>

#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(pop) 
#endif 

#include "as_function_params.hpp"
#include "as_object.hpp"
#include "as_value.hpp"
#include "swf_movie.hpp"
#include "swf_player.hpp"

namespace swf
{
	namespace
	{
		const std::string constructor_builtin_name = "__constructor__";
	}

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
		as_value_ptr value = player::get_builtin_object_method(name);
		if(value) {
			return value;
		}
		auto it = members_.find(name);
		if(it != members_.end()) {
			value = it->second;
		} else if(prototype_) {
			value = prototype_->get_member(name);
		}
		if(!value) {
			return as_value_ptr();
		}
		if(value->is_property()) {
			value->set_property_target(shared_from_this());
		}
		return value;
	}

	void as_object::set_member(const std::string& name, const as_value_ptr& value)
	{
		auto current_value = get_member(name);
		if(current_value) {
			if(current_value->is_property()) {
				current_value->set_property(value);
				return;
			}
		}
		// call if any watchers set on this value.
		call_watcher(name, current_value, value);

		auto it = members_.find(name);
		if(it != members_.end()) {
			if(it->second->is_readonly()) {
				return;
			}
			it->second = value;
		} else {
			members_[name] = value;
		}
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

	as_value_ptr as_object::get_constructor() const
	{
		auto it = members_.find(constructor_builtin_name);
		if(it != members_.end()) {
			return it->second;
		}
		return as_value_ptr();
	}

	void as_object::set_constructor(const as_value_ptr& ctor)
	{
		set_builtin_member(constructor_builtin_name, ctor);
	}

	void as_object::set_builtin_member(const std::string& name, const as_value_ptr& value)
	{
		value->set_flag(PropertyFlags::DO_NOT_ENUM);
		members_[name] = value->clone();
	}

	as_value_ptr as_object::add_property(const function_params& params)
	{
		if(params.arg_count() == 3) {
			ASSERT_LOG(params.that() != nullptr, "This pointer was null.");
			params.that()->set_builtin_member(params.arg(0)->to_std_string(), as_value::create(params.arg(1), params.arg(2)));
			return as_value::from_bool(true);
		} else {
			LOG_WARN("Not enough/Too many arguments in call to add_property_fn. Expected 3 found: " << params.arg_count());
		}
		return as_value::from_bool(false);
	}

	as_value_ptr as_object::register_class(const function_params& params)
	{
		bool result = false;
		auto target = params.get_environment()->get_target();
		if(params.arg_count() == 2 && target != nullptr) {
			std::string class_name = params.arg(0)->to_std_string();
			auto def = target->find_exported_resources(class_name);
			if(def) {
				as_function_ptr fn = params.arg(1)->to_function();
				if(fn) {
					LOG_DEBUG("register_class(overwriting): " << class_name);
					result = true;
					def->set_registered_class_constructor(fn);
				}
			} else {
				movie_def_ptr def = movie_def::create();
				auto movie = target->get_player()->get_root_movie();
				as_function_ptr fn = params.arg(1)->to_function();
				if(fn) {
					LOG_DEBUG("register_class(new): " << class_name);
					movie->export_resource(class_name, def);
					result = true;
					def->set_registered_class_constructor(fn);
				}
			}
		}
		return as_value::from_bool(result);
	}

	as_value_ptr as_object::has_own_property(const function_params& params)
	{
		bool result = false;
		if(params.arg_count() == 1) {
			ASSERT_LOG(params.that() != nullptr, "'this' pointer was null.");
			auto it = params.that()->members_.find(params.arg(0)->to_std_string());
			if(it != params.that()->members_.end()) {
				result = true;
			}
		}
		return as_value::from_bool(result);
	}

	as_value_ptr as_object::watch(const function_params& params)
	{
		bool result = false;
		if(params.arg_count() >= 2) {
			ASSERT_LOG(params.that() != nullptr, "'this' pointer was null.");
			std::string property_to_watch = params.arg(0)->to_std_string();
			result = params.that()->set_watch(property_to_watch, 
				params.arg(1)->to_function(), 
				params.arg_count() > 2 ? params.arg(2) : as_value::create());
		}
		return as_value::from_bool(result);
	}

	as_value_ptr as_object::unwatch(const function_params& params)
	{
		bool result = false;
		if(params.arg_count() >= 1) {
			ASSERT_LOG(params.that() != nullptr, "'this' pointer was null.");
			std::string property_to_unwatch = params.arg(0)->to_std_string();
			result = params.that()->remove_watch(property_to_unwatch);
		}
		return as_value::from_bool(result);
	}

	as_value_ptr as_object::add_event_listener(const function_params& params)
	{
		ASSERT_LOG(false, "XXX add_event_listener");
	}

	as_value_ptr as_object::dump(const function_params& params)
	{
		ASSERT_LOG(params.that() != nullptr, "'this' pointer was null.");
		LOG_DEBUG("XXX should dump object here");
		return nullptr;
	}

	bool as_object::set_watch(const std::string& name, as_function_ptr callback, as_value_ptr argument)
	{
		if(callback == nullptr) {
			return false;
		}
		watchers_[name] = as_watch(callback, argument);
		return true;
	}

	bool as_object::remove_watch(const std::string& name)
	{
		auto it = watchers_.find(name);
		if(it != watchers_.end()) {
			watchers_.erase(it);
			return true;
		}
		return false;
	}

	bool as_object::call_watcher(const std::string& name, as_value_ptr old_value, as_value_ptr new_value)
	{
		auto it = watchers_.find(name);
		if(it != watchers_.end()) {
			as_watch& w = it->second;
			if(w.callback_) {
				auto env = environment::create(get_player());
				env->push(w.arg_);
				env->push(new_value->clone());
				env->push(old_value);
				env->push(as_value::create(name));
				(*w.callback_)(function_params(get_this_value(), env, 4, env->get_top_index()));
			}
		}
		return false;
	}

	as_value_ptr as_object::get_this_value()
	{
		return as_value::create(shared_from_this());
	}

	const as_object_ptr& as_object::get_global() const
	{
		return get_player()->get_global();
	}
}
