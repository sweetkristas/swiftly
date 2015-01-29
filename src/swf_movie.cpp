#include <algorithm>
#include "swf_movie.hpp"

namespace swf
{
	movie_def::movie_def()
		: current_frame_(0),
		  max_frames_(0)
	{
	}

	movie_def::~movie_def()
	{
	}

	void movie_def::set_frame_count(int frame_count)
	{
		max_frames_ = frame_count;
		characters_.resize(frame_count);
		commands_.resize(frame_count);
	}

	void movie_def::set_frame_rate(float fr) 
	{ 
		frame_rate_ = std::min(120.0f, std::max(1.0f, fr)); 
	}

	void movie_def::add_character(int id, const character_def_ptr& ch)
	{
		ASSERT_LOG(get_current_frame() < static_cast<int>(characters_.size()), "Current frame exceeds the number of frames defined.");
		characters_[get_current_frame()][id] = ch;
	}

	void movie_def::show_frame()
	{
		++current_frame_;
	}

	void movie_def::add_command(const command_ptr& cmd)
	{
		ASSERT_LOG(get_current_frame() < static_cast<int>(characters_.size()), "Current frame exceeds the number of frames defined.");
		commands_[get_current_frame()].emplace_back(cmd);
	}

	character_def_ptr movie_def::get_character_def_from_id(int id)
	{
		auto it = characters_[get_current_frame()].find(id);
		ASSERT_LOG(it != characters_[get_current_frame()].end(), "Character with id: " << id << " not found in current frame: " << get_current_frame());
		return it->second;
	}

	void movie_def::set_frame_label(const std::string& label)
	{
		named_frames_[get_current_frame()] = label;
	}

	void movie_def::add_frame_label(unsigned frame, const std::string& label)
	{
		frame_label_[frame] = label;
	}

	void movie_def::add_scene_info(unsigned frame, const std::string& label)
	{
		scene_info_[frame] = label;
	}

	void movie_def::execute_commands(int frame, const character_ptr& ch)
	{
		ASSERT_LOG(frame < static_cast<int>(commands_.size()), "Tried to execute a frame beyond the maximum number of frames. " << frame << " >= " << commands_.size());
		ASSERT_LOG(ch != nullptr, "Tried to execute commands on null character.");
		for(auto& cmd : commands_[frame]) {
			cmd->execute(ch);
		}
	}

	movie::movie(weak_player_ptr player, const character_ptr& parent, int id, character_def_ptr def)
		: character(player, parent, id, def),
		  current_frame_(0)
	{
	}

	void movie::next_frame()
	{
		++current_frame_;
	}

	void movie::prev_frame()
	{
		--current_frame_;
	}

	void movie::draw() const
	{
		LOG_DEBUG("movie drawing frame: " << current_frame_);
	}

	void movie::update(float delta_time)
	{
		// on_load events, if not done and needed

		int prev_frame = current_frame_;
		if(++current_frame_ >= get_definition()->get_frame_count()) {
			current_frame_ = 0;
		}

		if(prev_frame != current_frame_) {
			get_definition()->execute_commands(current_frame_, shared_from_this());
		}
	}
}
