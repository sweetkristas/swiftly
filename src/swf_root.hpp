#pragma once

#include <boost/intrusive_ptr.hpp>
#include "ref_counted_ptr.hpp"

namespace swf
{
	class root : public reference_counted_ptr
	{
	public:
	private:
	};

	typedef boost::intrusive_ptr<root> root_ptr;
}
