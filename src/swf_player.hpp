#pragma once

#include "kre/SceneFwd.hpp"

#include "as_object.hpp"
#include "swf_root.hpp"
#include "swf_fwd.hpp"

namespace swf
{
	typedef std::function<void(KRE::SceneObjectPtr o)> render_attach_fn;
	typedef std::function<void(KRE::SceneObjectPtr o)> render_remove_fn;

	class player : public std::enable_shared_from_this<player>
	{
	public:
		MAKE_FACTORY(player);

		root_ptr load_file(const std::string& filename);

		void set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network);
		bool use_as3() const { return use_as3_; }
		bool use_direct_blit() const { return use_direct_blit_; }
		bool use_gpu() const { return use_gpu_; }
		bool use_network() const { return use_network_; }

		void set_transparent_background(bool en=true) { transparent_background_ = en; }
		void force_realtime_framerate(bool en=true) { realtime_framerate_ = en; }
		const root_ptr& get_root_movie() const { return root_; }

		const as_object_ptr& get_global() const;

		void set_version(int version) { version_ = version; }
		int get_version() const { return version_; }

		int get_twips() const { return 20; }

		void set_render_attach_fn(render_attach_fn attach_fn, render_remove_fn remove_fn);

		render_attach_fn get_render_attach_fn() const { return attach_fn_; }
		render_remove_fn get_render_remove_fn() const { return remove_fn_; }

		static as_value_ptr get_builtin_object_method(const std::string& name);
		static as_value_ptr get_builtin_sprite_method(const std::string& name);
		static as_value_ptr get_builtin_numeric_method(const std::string& name);
		static as_value_ptr get_builtin_boolean_method(const std::string& name);
		static as_value_ptr get_builtin_string_method(const std::string& name);
	private:
		player();
		void init();

		// file attributes
		bool use_direct_blit_;
		bool use_gpu_;
		bool use_as3_;
		bool use_network_;

		as_object_ptr global_;
		root_ptr root_;

		bool transparent_background_;
		bool realtime_framerate_;
		int version_;

		render_attach_fn attach_fn_;
		render_remove_fn remove_fn_;
	};
}
