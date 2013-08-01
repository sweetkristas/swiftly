#include "swf_text.hpp"

namespace swf 
{
	text::text() : character()
	{
	}

	text::~text()
	{
	}

	void text::read(bit_stream_ptr bits)
	{
		set_id(bits->read_unsigned16());
		text_bounds_ = bits->read_rect();
		text_matrix_ = bits->read_matrix();
		unsigned glyph_bits = bits->read_unsigned8();
		unsigned advance_bits = bits->read_unsigned8();

		bool finished = false;
		while(!finished) {
			uint8_t flags = bits->read_unsigned8();
			if(flags == 0) {
				finished = true;
			} else {
				if(flags & 8) {
					// has font
					font_id_ = bits->read_unsigned16();
				}
				if(flags & 4) {
					// has color
					// This is an rgba record for DefineFont2
					rgb color = bits->read_rgb();
					color_.r = color.r;
					color_.g = color.g;
					color_.b = color.b;
					color_.a = 255;
				}
				if(flags & 1) {
					// Has X offset
					x_offset_ = bits->read_signed16();
					
				}
				if(flags & 2) {
					// Has Y offset
					y_offset_ = bits->read_signed16();
				}
				if(flags & 8) {
					text_height_ = bits->read_signed16();
				}
				glyph_indices_.clear();
				glyph_advance_.clear();
				unsigned glyph_count = bits->read_unsigned8();
				for(int n = 0; n != glyph_count; ++n) {
					glyph_indices_.push_back(bits->read_unsigned_bits(glyph_bits));
					glyph_advance_.push_back(bits->read_signed_bits(advance_bits));
				}
			}
		}
	}

	void text::draw() const
	{
	}
}
