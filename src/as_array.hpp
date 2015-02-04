#pragma once

#include "as_object.hpp"

namespace swf
{
	enum class SortOptions
	{
		NONE					= 0x00,
		CASE_INSENSITIVE		= 0x01,
		DESCENDING				= 0x02,
		UNIQUE					= 0x04,
		RETURN_INDEXED_ARRAY	= 0x08,
		NUMERIC					= 0x10,
	};

	inline bool operator&(SortOptions s1, SortOptions s2) {
		return (static_cast<unsigned>(s1) & static_cast<unsigned>(s2)) != 0;
	}

	class as_array : public as_object
	{
	public:
		MAKE_FACTORY(as_array);

		typedef std::vector<as_value_ptr>::iterator iterator;
		typedef std::vector<as_value_ptr>::const_iterator const_iterator;

		virtual bool is_a(ASClass id) override  {
			return id == ASClass::ARRAY ? true : as_object::is_a(id);
		}
		void sort(SortOptions opts, std::function<bool(const as_value_ptr&,const as_value_ptr&)> cmp_fn);
		int size() const { return static_cast<int>(ary_.size()); }

		iterator begin() { return ary_.begin(); }
		iterator end() { return ary_.end(); }
		const_iterator begin() const { return ary_.begin(); }
		const_iterator end() const { return ary_.end(); }

		void set_value(int n, const as_value_ptr& value);
		as_value_ptr get_value(int n) const;

		as_value_ptr get_member(const std::string& name);
		void set_member(const std::string& name, const as_value_ptr& value);

		static as_value_ptr array_constructor(const function_params& params);

		static as_value_ptr array_join(const function_params& params);
		static as_value_ptr array_concat(const function_params& params);
		static as_value_ptr array_slice(const function_params& params);
		static as_value_ptr array_unshift(const function_params& params);
		static as_value_ptr array_sort(const function_params& params);
		static as_value_ptr array_shift(const function_params& params);
		static as_value_ptr array_to_string(const function_params& params);
		static as_value_ptr array_push(const function_params& params);
		static as_value_ptr array_pop(const function_params& params);
		static as_value_ptr array_length(const function_params& params);
		static as_value_ptr array_splice(const function_params& params);

	private:
		as_array(weak_player_ptr player);
		void init();

		std::vector<as_value_ptr> ary_;
	};

	typedef std::shared_ptr<as_array> as_array_ptr;
}
