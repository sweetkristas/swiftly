#include "swf_font.hpp"

namespace swf
{
	font_def::font_def()
	{
	}

	font_def::~font_def()
	{
	}

	void font_def::read1(bit_stream_ptr bits)
	{
		int num_table_entries = static_cast<int>(bits->read_unsigned16()) / 2;
		// read (and discard) the offset entries.
		for(int n = 0; n != num_table_entries; ++n) {
			bits->read_unsigned16();
		}
		glyphs_.clear();
		for(int n = 0; n != num_table_entries; ++n) {
			auto shp = shape_def::create();
			shp->read(bits);
			glyphs_.emplace_back(shp);
		}
	}

	void font_def::read(int vers, bit_stream_ptr bits)
	{
		if(vers == 1) {
			// It's different enough to justify a seperate function.
			return read1(bits);
		}
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
		font_name_.clear();
		for(unsigned n = 0; n != font_name_length; ++n) {
			font_name_ += char(bits->read_unsigned8());
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
		glyphs_.clear();
		for(unsigned n = 0; n != glyph_count; ++n) {
			auto shp = shape_def::create();
			shp->read(bits);
			glyphs_.emplace_back(shp);
		}
		// Read ucs-2 codes table
		code_points_.clear();
		for(unsigned n = 0; n != glyph_count; ++n) {
			uint16_t code = wide_codes ? bits->read_unsigned16() : bits->read_unsigned8();
			code_points_.emplace_back(code);
		}
		if(has_layout) {
			font_ascender_height_ = bits->read_unsigned16();
			font_descender_height_ = bits->read_unsigned16();
			font_leading_height_ = bits->read_signed16();
			font_advance_table_.clear();
			for(unsigned n = 0; n != glyph_count; ++n) {
				font_advance_table_.emplace_back(bits->read_signed16());
			}
			font_bounds_table_.clear();
			for(unsigned n = 0; n != glyph_count; ++n) {
				font_bounds_table_.emplace_back(bits->read_rect());
			}
			unsigned kerning_count = bits->read_unsigned16();
			kerning_records_.clear();
			for(unsigned n = 0; n != kerning_count; ++n) {				
				uint16_t cp0 = wide_codes ? bits->read_unsigned16() : bits->read_unsigned8();
				uint16_t cp1 = wide_codes ? bits->read_unsigned16() : bits->read_unsigned8();
				kerning_records_[std::make_pair(cp0,cp1)] = bits->read_signed16();
			}
		}
	}

	font::font(player_ptr player, const character_ptr& parent, int id, const font_def_ptr& def)
		: character(player, parent, id, def)
	{
	}
}
