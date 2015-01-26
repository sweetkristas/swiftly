#pragma once 

#include <map>
#include <vector>

#include "as_object.hpp"
#include "swf_tags.hpp"

namespace swf
{
	typedef std::map<int, std::string> frame_name_map;

	class movie_def : public as_object_def
	{
	public:
		explicit movie_def(int frame_count);
		virtual ~movie_def();

		bool is(ASClass id) override { return id == ASClass::MOVIE_DEF; }

		void set_frame_count(int frame_count);

		void set_frame(int f) { current_frame_ = f; }
		int get_frame() const { return current_frame_; }

		virtual bool is_tag_allowable(Tag tag) = 0;
	private:
		int current_frame_;
		int max_frames_;
		frame_name_map named_frames_;
	};
}
