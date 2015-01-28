#pragma once

#include "bit_reader.hpp"
#include "swf_character.hpp"
#include "swf_filter.hpp"

namespace swf 
{
	struct button_record
	{
		uint8_t blend_mode_;
		filter_list filter_list_;
		bool on_hit_test_;
		bool on_down_;
		bool on_over_;
		bool on_up_;
		uint16_t character_id_;
		uint16_t depth_;
		matrix2x3 transform_;
		color_transform ct_;
	};

	std::vector<button_record> read_button_records(int version, bit_stream_ptr bits);

	class button_def : public character_def
	{
	public:
		MAKE_FACTORY(button_def);

		virtual bool is_a(ASClass id) override  { return id == ASClass::BUTTON_DEF ? true : character_def::is_a(id); }

		void read2(bit_stream_ptr bits);
	private:
		button_def();
		// true to track as a menu button, false to track as a menu button
		bool track_as_menu_button_;	
		std::vector<button_record> button_records_;;

		button_def(const button_def&) = delete;
		void operator=(const button_def&) = delete;
	};

	typedef std::shared_ptr<button_def> button_def_ptr;

	class button : public character
	{
	public:
		button(player_ptr player, const character_ptr& parent, int id, const button_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::BUTTON ? true : character::is_a(id); }		
	private:
		button_def_ptr def_;
	};
}
