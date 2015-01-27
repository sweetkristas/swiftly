#pragma once 

#include <map>
#include <vector>

#include "as_object.hpp"
#include "swf_character.hpp"
#include "swf_command.hpp"
#include "swf_tags.hpp"

namespace swf
{
	typedef std::map<int, std::string> frame_name_map;

	typedef std::map<int, character_def_ptr> character_id_map;

	class movie_def : public as_object_def
	{
	public:
		MAKE_FACTORY(movie_def);
		virtual ~movie_def();

		bool is_a(ASClass id) override { return id == ASClass::MOVIE_DEF; }

		void set_frame_count(int frame_count);

		void set_current_frame(int f) { current_frame_ = f; }
		int get_current_frame() const { return current_frame_; }

		const rect& get_frame_size() const { return frame_size_; }
		void set_frame_size(const rect& r) { frame_size_ = r; }

		float get_frame_rate() const { return frame_rate_; }
		void set_frame_rate(float fr);

		virtual bool is_tag_allowable(Tag tag) { return true; }

		void show_frame();

		void add_character(int id, const character_def_ptr& ch);
		void add_command(const command_ptr& cmd);
	private:
		explicit movie_def();

		int current_frame_;
		int max_frames_;
		frame_name_map named_frames_;

		rect frame_size_;
		float frame_rate_;

		std::vector<character_id_map> characters_;
		std::vector<std::vector<command_ptr>> commands_;
	};

	class movie : public character
	{
	public:
		MAKE_FACTORY(movie);
	private:
		explicit movie(weak_player_ptr player, movie_def_ptr def);
	};
}
