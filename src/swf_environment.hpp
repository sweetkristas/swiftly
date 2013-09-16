#pragma once

#include <deque>

#include "as3/as3_value.hpp"

namespace swf
{
	typedef std::deque<avm2::as3_value_ptr> stack;
	typedef std::deque<avm2::as3_value_ptr>::iterator stack_iterator;

	class environment
	{
	public:
		environment() {
		}
		virtual ~environment() {
		}
	private:
		stack args_;
	};
}
