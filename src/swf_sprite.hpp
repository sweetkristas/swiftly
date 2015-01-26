#pragma once

#include <memory>
#include <vector>

#include "swf_movie.hpp"

namespace swf
{
	class sprite : public movie
	{
	public:
		sprite(int id, int frame_cnt);

		bool is_tag_allowable(Tag tag) override;
	private:
		int id_;
		// std::vector<action_ptr> actions_;
		// std::vector<action_ptr> init_actions_;
		// bool done_init_actions_;
	};
}
