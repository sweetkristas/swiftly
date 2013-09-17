#include "asserts.hpp"
#include "as3/as3_function_call.hpp"
#include "as3/as3_globals.hpp"
#include "swf_player.hpp"

namespace swf
{
	player::player()
	{
		global_ = new avm2::as3_object(this);
		
		init();
	}

	player::~player()
	{
	}

	void player::init()
	{
		ASSERT_LOG(global_ != NULL, "player::init(): global_ is NULL");
		global_->builtin("trace", avm2::as3_trace_function);
		//global_->builtin("Object");
		//global_->builtin("Sound");
	}
}