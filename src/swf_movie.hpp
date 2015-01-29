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

	class movie_def : public character_def
	{
	public:
		MAKE_FACTORY(movie_def);
		virtual ~movie_def();

		bool is_a(ASClass id) override { return id == ASClass::MOVIE_DEF ? true : character_def::is_a(id); }

		void set_frame_count(int frame_count);

		void set_current_frame(int f) { current_frame_ = f; }
		int get_current_frame() const { return current_frame_; }

		const rect& get_frame_size() const { return frame_size_; }
		void set_frame_size(const rect& r) { frame_size_ = r; }

		float get_frame_rate() const { return frame_rate_; }
		void set_frame_rate(float fr);

		void show_frame() override;

		void add_character(int id, const character_def_ptr& ch) override;
		void add_command(const command_ptr& cmd) override;

		character_def_ptr get_character_def_from_id(int id);
		void set_frame_label(const std::string& label) override;
		
		void add_frame_label(unsigned frame, const std::string& label) override;
		void add_scene_info(unsigned frame, const std::string& label) override;

		int get_frame_count() const override { return max_frames_; }

		void execute_commands(int frame, const character_ptr& ch) override;
	protected:
		explicit movie_def();
	private:
		int current_frame_;
		int max_frames_;
		frame_name_map named_frames_;

		rect frame_size_;
		float frame_rate_;

		std::vector<character_id_map> characters_;
		std::vector<std::vector<command_ptr>> commands_;
		std::map<unsigned, std::string> frame_label_;
		std::map<unsigned, std::string> scene_info_;
	};

	class movie : public character
	{
	public:
		MAKE_FACTORY(movie);

		bool is_a(ASClass id) override { return id == ASClass::MOVIE ? true : character::is_a(id); }

		void next_frame();
		void prev_frame();

		void update(float delta_time) override;
		void draw() const override;
	protected:
		explicit movie(weak_player_ptr player, const character_ptr& parent, int id, character_def_ptr def);
	private:
		int current_frame_;
	};
}
