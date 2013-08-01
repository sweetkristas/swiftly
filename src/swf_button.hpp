#pragma once

#include "bit_reader.hpp"
#include "swf.hpp"
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

	class button : public character
	{
	public:
		button();
		virtual ~button();

		void read2(bit_stream_ptr bits);

		void draw() const;
	private:
		// true to track as a menu button, false to track as a menu button
		bool track_as_menu_button_;	
		std::vector<button_record> button_records_;;

		button(const button&);
	};
}
