#pragma once

#include "swf_fwd.hpp"

namespace swf
{
	class code_block
	{
	public:
		explicit code_block(const codestream& codes, codestream_iterator start, codestream_iterator end)
			: codestream_(codes),
			  start_(start),
			  end_(end),
			  ip_(start)
		{
		}
		explicit code_block(const codestream& codes) 
			: codestream_(codes),
			  start_(codes.begin()),
			  end_(codes.end()),
			  ip_(codes.begin())
		{
		}
	private:
		codestream codestream_;
		codestream_iterator start_;
		codestream_iterator end_;
		codestream_iterator ip_;
	};
}
