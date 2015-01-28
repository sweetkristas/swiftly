#include "swf_text.hpp"

namespace swf 
{
	text_def::text_def()
		: text_bounds_(),
		  text_matrix_(),
		  x_offset_(0),
		  y_offset_(0),
		  color_(),
		  text_height_(0),
		  render_type_(0),
		  grid_fit_(0),
		  thickness_(0),
		  sharpness_(0)
	{
	}

	text_def::~text_def()
	{
	}

	void text_def::read(bit_stream_ptr bits)
	{
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

	text::text(player_ptr player, const character_ptr& parent, int id, const text_def_ptr& def)
		: character(player, parent, id, def)
	{
	}
}
