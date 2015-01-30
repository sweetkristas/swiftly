#include "asserts.hpp"
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
		LOG_DEBUG("text_def: bounds: " << text_bounds_ << ", matrix: " << text_matrix_);
	}

	character_ptr text_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		return std::make_shared<text>(player, parent, id, shared_from_this());
	}

	text::text(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: character(player, parent, id, def)
	{
	}

	void text::handle_draw() const
	{
		LOG_DEBUG("drawing text: " << get_id());
	}

	edit_text_def::edit_text_def()
		: bounds_(),
		  has_text_(false),
		  has_word_wrap_(false),
		  is_multiline_(false),
		  is_password_(false),
		  is_readonly_(false),
		  has_text_color_(false),
		  has_max_length_(false),
		  has_font_(false),
		  has_font_class_(false),
		  is_auto_size_(false),
		  has_layout_(false),
		  allow_select_(false),
		  has_border_(false),
		  is_static_(false),
		  has_html_(false),
		  use_outlines_(false),
		  font_id_(0),
		  font_class_(),
		  font_height_(0),
		  font_color_(),
		  max_length_(0),
		  text_align_(TextAlign::LEFT),
		  left_margin_(0),
		  right_margin_(0),
		  indent_(0),
		  leading_(0)	  
	{
	}

	void edit_text_def::read(bit_stream_ptr bits)
	{
		bounds_ = bits->read_rect();
		has_text_ = bits->read_unsigned_bits(1) != 0;
		has_word_wrap_ = bits->read_unsigned_bits(1) != 0;
		is_multiline_ = bits->read_unsigned_bits(1) != 0;
		is_password_ = bits->read_unsigned_bits(1) != 0;
		is_readonly_ = bits->read_unsigned_bits(1) != 0;
		has_text_color_ = bits->read_unsigned_bits(1) != 0;
		has_max_length_ = bits->read_unsigned_bits(1) != 0;
		has_font_ = bits->read_unsigned_bits(1) != 0;
		has_font_class_ = bits->read_unsigned_bits(1) != 0;
		is_auto_size_ = bits->read_unsigned_bits(1) != 0;
		has_layout_ = bits->read_unsigned_bits(1) != 0;
		allow_select_ = bits->read_unsigned_bits(1) != 0;
		has_border_ = bits->read_unsigned_bits(1) != 0;
		is_static_ = bits->read_unsigned_bits(1) != 0;
		has_html_ = bits->read_unsigned_bits(1) != 0;
		use_outlines_ = bits->read_unsigned_bits(1) != 0;

		if(has_font_) {
			font_id_ = bits->read_unsigned16();
		}
		if(has_font_class_) {
			font_class_ = bits->read_string();
		}
		if(has_font_) {
			font_height_ = bits->read_unsigned16();
		}
		if(has_text_color_) {
			font_color_ = bits->read_rgba();
		}
		if(has_max_length_) {
			max_length_ = bits->read_unsigned16();
		}
		if(has_layout_) {
			text_align_ = static_cast<TextAlign>(bits->read_unsigned8());
			left_margin_ = bits->read_unsigned16();
			right_margin_ = bits->read_unsigned16();
			indent_ = bits->read_unsigned16();
			leading_ = bits->read_signed16();
		}
		variable_name_ = bits->read_string();
		if(has_text_) {
			initial_text_ = bits->read_string();
		}

		LOG_DEBUG("edit_text: variable_name: " << variable_name_);
	}

	character_ptr edit_text_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		return std::make_shared<edit_text>(player, parent, id, shared_from_this());
	}

	edit_text::edit_text(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: character(player, parent, id, def)
	{
	}

	void edit_text::handle_draw() const
	{
		LOG_DEBUG("drawing edit_text: " << get_id());
	}

}
