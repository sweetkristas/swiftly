#include "as3_object.hpp"

namespace avm2
{
	namespace
	{
		as3_object_ptr get_global()
		{
			static as3_object_ptr res;
			if(res == NULL) {
				res.reset(new as3_object());
			}
		}
	}

	as3_object_ptr as3_object::get_global()
	{
	}
}
