#pragma once

#include "ref_counted_ptr.hpp"
#include "as3/as3_object.hpp"
#include "swf_root.hpp"

namespace swf
{
	class player : public reference_counted_ptr
	{
	public:
		player();
		virtual ~player();
	private:
		void init();

		avm2::as3_object_ptr global_;
		root_ptr root_;
	};
	typedef boost::intrusive_ptr<player> player_ptr;
}
