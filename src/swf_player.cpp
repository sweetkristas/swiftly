#include "asserts.hpp"
#include "as_object.hpp"
#include "as_value.hpp"
#include "swf_movie.hpp"
#include "swf_player.hpp"
#include "swf_reader.hpp"

namespace swf
{
	namespace
	{
		typedef std::map<std::string, as_value_ptr> builtin_map_type;

		void init_object_methods(builtin_map_type* res)
		{
			(*res)["addProperty"] = as_value::create(as_object::add_property);
			(*res)["registerClass"] = as_value::create(as_object::register_class);
			(*res)["hasOwnProperty"] = as_value::create(as_object::has_own_property);
			(*res)["watch"] = as_value::create(as_object::watch);
			(*res)["unwatch"] = as_value::create(as_object::unwatch);
			(*res)["addEventListener"] = as_value::create(as_object::add_event_listener);
			(*res)["addEventListener"] = as_value::create(as_object::dump);
		}

		void init_sprite_methods(builtin_map_type* res)
		{
			/*
			(*res)["play"] = as_value::create(sprite_play);
			(*res)["stop"] = as_value::create(sprite_stop);
			(*res)["gotoAndStop"] = as_value::create(sprite_goto_and_stop);
			(*res)["gotoAndPlay"] = as_value::create(sprite_goto_and_play);
			(*res)["nextFrame"] = as_value::create(sprite_next_frame);
			(*res)["prevFrame"] = as_value::create(sprite_prev_frame);
			(*res)["getBytesLoaded"] = as_value::create(sprite_get_bytes_loaded);
			(*res)["getBytesTotal"] = as_value::create(sprite_get_bytes_total);
			(*res)["swapDepths"] = as_value::create(sprite_swap_depths);
			(*res)["duplicateMovieClip"] = as_value::create(sprite_duplicate_movieclip);
			(*res)["getDepth"] = as_value::create(sprite_get_depth);
			(*res)["createEmptyMovieClip"] = as_value::create(sprite_create_empty_movieclip);
			(*res)["removeMovieClip"] = as_value::create(sprite_remove_movieclip);
			(*res)["hitTest"] = as_value::create(sprite_hit_test);
			(*res)["startDrag"] = as_value::create(sprite_start_drag);
			(*res)["stopDrag"] = as_value::create(sprite_stop_drag);
			(*res)["loadMovie"] = as_value::create(sprite_loadmovie);
			(*res)["unloadMovie"] = as_value::create(sprite_unloadmovie);
			(*res)["getNextHighestDepth"] = as_value::create(sprite_get_next_highest_depth);
			(*res)["createTextField"] = as_value::create(sprite_create_text_field);
			(*res)["attachMovie"] = as_value::create(sprite_attach_movie);
			(*res)["localToGlobal"] = as_value::create(sprite_local_to_global);
			(*res)["globalToLocal"] = as_value::create(sprite_global_to_local);
			(*res)["getRect"] = as_value::create(sprite_get_rect);
			(*res)["getBounds"] = as_value::create(sprite_get_bounds);
			(*res)["setMask"] = as_value::create(sprite_set_mask);
			(*res)["beginFill"] = as_value::create(sprite_begin_fill);
			(*res)["endFill"] = as_value::create(sprite_end_fill);
			(*res)["lineTo"] = as_value::create(sprite_line_to);
			(*res)["moveTo"] = as_value::create(sprite_move_to);
			(*res)["curveTo"] = as_value::create(sprite_curve_to);
			(*res)["clear"] = as_value::create(sprite_clear);
			(*res)["lineStyle"] = as_value::create(sprite_line_style);
			(*res)["addFrameScript"] = as_value::create(sprite_add_frame_script);
			*/
		}

		void init_numeric_methods(builtin_map_type* res)
		{
			/*
			(*res)["toString"] = as_value::create(as_numeric_to_string);
			(*res)["valueOf"] = as_value::create(as_numeric_value_of);
			*/
		}

		void init_boolean_methods(builtin_map_type* res)
		{
			/*
			(*res)["toString"] = as_value::create(as_bool_to_string);
			(*res)["valueOf"] = as_value::create(as_bool_value_of);
			*/
		}

		void init_string_methods(builtin_map_type* res)
		{
			/*
			(*res)["toString"] = as_value::create(as_string_to_string);
			(*res)["fromCharCode"] = as_value::create(as_string_from_char_code);
			(*res)["charCodeAt"] = as_value::create(as_string_char_code_at);
			(*res)["concat"] = as_value::create(as_string_concat);
			(*res)["indexOf"] = as_value::create(as_string_index_of);
			(*res)["lastIndexOf"] = as_value::create(as_string_last_index_of);
			(*res)["slice"] = as_value::create(as_string_slice);
			(*res)["split"] = as_value::create(as_string_split);
			(*res)["substring"] = as_value::create(as_string_substring);
			(*res)["substr"] = as_value::create(as_string_substr);
			(*res)["toLowerCase"] = as_value::create(as_string_to_lowercase);
			(*res)["toUpperCase"] = as_value::create(as_string_to_uppercase);
			(*res)["charAt"] = as_value::create(as_string_char_at);
			(*res)["length"] = as_value::create(as_string_length);
			*/
		}
	}

	player::player()
		: use_direct_blit_(false),
		  use_gpu_(false),
		  use_as3_(false),
		  use_network_(false),
		  transparent_background_(false),
		  realtime_framerate_(false),
		  version_(0)
	{
	}

	void player::init()
	{
		global_ = as_object::create(shared_from_this());
		ASSERT_LOG(global_ != NULL, "player::init(): global_ is NULL");
		//global_->builtin("trace", avm2::as3_value(avm2::as3_trace_function));
		//global_->builtin("Object");
		//global_->builtin("Sound");
	}

	root_ptr player::load_file(const std::string& filename)
	{
		auto m = movie_def::create();
		root_ = root::create(shared_from_this(), m);
		reader swf_reader(filename, shared_from_this());
		return root_;
	}
	
	void player::set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network)
	{
		use_direct_blit_ = use_direct_blit;
		use_gpu_ = use_gpu;
		use_as3_ = use_as3;
		use_network_ = use_network;
	}

	as_value_ptr player::get_builtin_object_method(const std::string& name)
	{
		static builtin_map_type res;
		if(res.empty()) {
			init_object_methods(&res);
		}
		auto it = res.find(name);
		if(it == res.end()) {
			return as_value_ptr();
		}
		return it->second;
	}

	as_value_ptr player::get_builtin_sprite_method(const std::string& name)
	{
		static builtin_map_type res;
		if(res.empty()) {
			init_sprite_methods(&res);
		}
		auto it = res.find(name);
		if(it == res.end()) {
			return as_value_ptr();
		}
		return it->second;
	}

	as_value_ptr player::get_builtin_numeric_method(const std::string& name)
	{
		static builtin_map_type res;
		if(res.empty()) {
			init_numeric_methods(&res);
		}
		auto it = res.find(name);
		if(it == res.end()) {
			return as_value_ptr();
		}
		return it->second;
	}

	as_value_ptr player::get_builtin_boolean_method(const std::string& name)
	{
		static builtin_map_type res;
		if(res.empty()) {
			init_boolean_methods(&res);
		}
		auto it = res.find(name);
		if(it == res.end()) {
			return as_value_ptr();
		}
		return it->second;
	}

	as_value_ptr player::get_builtin_string_method(const std::string& name)
	{
		static builtin_map_type res;
		if(res.empty()) {
			init_string_methods(&res);
		}
		auto it = res.find(name);
		if(it == res.end()) {
			return as_value_ptr();
		}
		return it->second;
	}

	const as_object_ptr& player::get_global() const
	{
		return global_;
	}
}