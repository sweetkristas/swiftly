#include <algorithm>

#include "as_value.hpp"
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

	void movie_def::set_frame_label(const std::string& label)
	{
		named_frames_[label] = get_current_frame();
	}

	int movie_def::get_frame_from_label(const std::string& label) 
	{
		auto it = named_frames_.find(label);
		ASSERT_LOG(it != named_frames_.end(), "Couldn't find frame with label: '" << label << "'");
		return it->second;
	}

	void movie_def::add_frame_label(unsigned frame, const std::string& label)
	{
		frame_label_[frame] = label;
	}

	void movie_def::add_scene_info(unsigned frame, const std::string& label)
	{
		scene_info_[frame] = label;
	}

	void movie_def::execute_commands(int frame, const character_ptr& ch, bool actions_only)
	{
		ASSERT_LOG(frame < static_cast<int>(commands_.size()), "Tried to execute a frame beyond the maximum number of frames. " << frame << " >= " << commands_.size());
		ASSERT_LOG(ch != nullptr, "Tried to execute commands on null character.");
		for(auto& cmd : commands_[frame]) {
			if((actions_only && cmd->is_action()) || !actions_only) {
				cmd->execute(ch);
			}
		}
	}

	character_def_ptr movie_def::get_character_def_from_id(int frame, int id)
	{
		ASSERT_LOG(frame < static_cast<int>(characters_.size()), "Tried to find a character beyond the maximum number of frames. " << frame << " >= " << characters_.size());
		auto it = characters_[frame].find(id);
		ASSERT_LOG(it != characters_[frame].end(), "Unable to find character with id: " << id << " in frame: " << frame);
		return it->second;
	}

	character_ptr movie_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		return movie::create(player, parent, id, shared_from_this());
	}

	movie::movie(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
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

	void movie::handle_draw() const
	{
		LOG_DEBUG("movie drawing frame: " << current_frame_);
	}

	void movie::execute_actions()
	{

	}

	void movie::call_frame_actions(const as_value_ptr& val)
	{
		int frame = -1;
		if(val->is_numeric()) {
			// 1-based to 0-based conversion.
			frame = val->to_integer() - 1;
		} else if(val->is_string()) {
			frame = get_definition()->get_frame_from_label(val->to_std_string());
		}
		if(frame < 0 || frame >= static_cast<int>(get_definition()->get_frame_count())) {
			ASSERT_LOG(false, "frame outside limits: " << frame);
		}
		get_definition()->execute_commands(frame, get_character_ptr(), true);
	}

	void movie::update(float delta_time)
	{
		// on_load events, if not done and needed

		int prev_frame = current_frame_;
		if(++current_frame_ >= get_definition()->get_frame_count()) {
			current_frame_ = 0;
		}

		//if(prev_frame != current_frame_) {
			get_definition()->execute_commands(current_frame_, get_character_ptr());
		//}

		update_display_list(delta_time);

		execute_actions();
	}
}
