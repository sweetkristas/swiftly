#pragma once

#include "as_object.hpp"
#include "swf_root.hpp"
#include "swf_fwd.hpp"

namespace swf
{
	class player : public std::enable_shared_from_this<player>
	{
	public:
		MAKE_FACTORY(player);

		root_ptr load_file(const std::string& filename);

		void set_transparent_background(bool en=true) { transparent_background_ = en; }
		void force_realtime_framerate(bool en=true) { realtime_framerate_ = en; }
		const root_ptr& get_root_movie() const { return root_; }

		void set_version(int version) { version_ = version; }
		int get_version() const { return version_; }
	private:
		player();
		void init();

		as_object_ptr global_;
		root_ptr root_;

		bool transparent_background_;
		bool realtime_framerate_;
		int version_;
	};
}
