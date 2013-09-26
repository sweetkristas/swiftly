#pragma once

#include <stack>
#include "../avm2_fwd.hpp"
#include "as3_value.hpp"

namespace avm2
{
	class environment
	{
	public:
		environment();
		virtual ~environment();

		void execute(code_type_iterator code);
	private:
		std::stack<as3_value_ptr> stack_;
	};
}
