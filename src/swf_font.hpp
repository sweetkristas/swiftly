#pragma once

#include "bit_reader.hpp"
#include "swf.hpp"

namespace swf
{
	struct kerning_record
	{
		uint16_t code[2];
		int16_t adjust;
	};

	class font : public character
	{
	public:
		font();
		virtual ~font();

		void read3(std::shared_ptr<bit_stream> bits);

		void draw() const;
	private:
		uint8_t language_code_;
		std::vector<uint16_t> code_points_;
		uint16_t font_ascender_height_;
		uint16_t font_descender_height_;
		int16_t font_leading_height_;
		std::vector<int16_t> font_advance_table_;
		std::vector<rect> font_bounds_table_;
		std::vector<kerning_record> kerning_records_;
		std::vector<shape> glyphs_;

		font(const font&);
	};

	typedef boost::intrusive_ptr<font> font_ptr;
}
