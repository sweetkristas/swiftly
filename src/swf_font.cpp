#include "swf_font.hpp"

namespace swf
{
	font::font() : character()
	{
	}

	font::~font()
	{
	}

	void font::read3(std::shared_ptr<bit_stream> bits)
	{
		set_id(bits->read_unsigned16());
		bool has_layout = bits->read_unsigned_bits(1) ? true : false;
		bool shift_jis = bits->read_unsigned_bits(1) ? true : false;
		bool small_text = bits->read_unsigned_bits(1) ? true : false;
		bool flag_ansi = bits->read_unsigned_bits(1) ? true : false;
		bool wide_offsets = bits->read_unsigned_bits(1) ? true : false;
		bool wide_codes = bits->read_unsigned_bits(1) ? true : false;
		bool italic = bits->read_unsigned_bits(1) ? true : false;
		bool bold = bits->read_unsigned_bits(1) ? true : false;
		language_code_ = bits->read_unsigned8();
		unsigned font_name_length = unsigned(bits->read_unsigned8());
		std::string s;
		for(unsigned n = 0; n != font_name_length; ++n) {
			s += char(bits->read_unsigned8());
		}
		unsigned glyph_count = unsigned(bits->read_unsigned16());
		// read, and discard, offset table
		for(unsigned n = 0; n != glyph_count; ++n) {
			if(wide_offsets) {
				bits->read_unsigned32();
			} else {
				bits->read_unsigned16();
			}
		}
		uint32_t code_table_offset = wide_offsets ? bits->read_unsigned32() : uint32_t(bits->read_unsigned16());
		// Read glyph shapes.
		for(unsigned n = 0; n != glyph_count; ++n) {
			glyphs_.push_back(bits->read_shape());
		}
		// Read ucs-2 codes table
		for(unsigned n = 0; n != glyph_count; ++n) {
			code_points_.push_back(bits->read_unsigned16());
		}
		if(has_layout) {
			font_ascender_height_ = bits->read_unsigned16();
			font_descender_height_ = bits->read_unsigned16();
			font_leading_height_ = bits->read_signed16();
			for(unsigned n = 0; n != glyph_count; ++n) {
				font_advance_table_.push_back(bits->read_signed16());
			}
			for(unsigned n = 0; n != glyph_count; ++n) {
				font_bounds_table_.push_back(bits->read_rect());
			}
			unsigned kerning_count = bits->read_unsigned16();
			for(unsigned n = 0; n != kerning_count; ++n) {
				kerning_record kr;
				kr.code[0] = wide_codes ? bits->read_unsigned16() : bits->read_unsigned8();
				kr.code[1] = wide_codes ? bits->read_unsigned16() : bits->read_unsigned8();
				kr.adjust = bits->read_signed16();
				kerning_records_.push_back(kr);
			}
		}
	}

	void font::draw() const
	{
	}
}