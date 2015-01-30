#pragma once

#include "bit_reader.hpp"
#include "swf_character.hpp"
#include "swf_types.hpp"
#include "swf_player.hpp"

namespace swf
{
	enum class TextAlign {
		LEFT,
		RIGHT,
		CENTER,
		JUSTIFY,
	};

	class edit_text_def : public character_def
	{
	public:
		MAKE_FACTORY(edit_text_def);
		
		void read(bit_stream_ptr bits);
		virtual bool is_a(ASClass id) override  { return id == ASClass::EDIT_TEXT_DEF ? true : character_def::is_a(id); }
		character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id);
	private:
		edit_text_def();

		rect bounds_;
		bool has_text_;
		bool has_word_wrap_;
		bool is_multiline_;
		bool is_password_;
		bool is_readonly_;
		bool has_text_color_;
		bool has_max_length_;
		bool has_font_;
		bool has_font_class_;
		bool is_auto_size_;
		bool has_layout_;
		bool allow_select_;
		bool has_border_;
		bool is_static_;
		bool has_html_;
		bool use_outlines_;
		int font_id_;
		std::string font_class_;
		int font_height_; // in twips
		rgba font_color_;
		int max_length_; // max text length
		TextAlign text_align_;
		int left_margin_; // in twips
		int right_margin_; // in twips
		int indent_; // in twips
		int leading_; // vertical distance between bottom of descender and top of ascender on next line
		std::string variable_name_; 
		std::string initial_text_;
	};

	typedef std::shared_ptr<edit_text_def> edit_text_def_ptr;

	class edit_text : public character
	{
	public:
		edit_text(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::EDIT_TEXT ? true : character::is_a(id); }
	private:
		void handle_draw() const override;
	};

	class text_def : public character_def
	{
	public:
		MAKE_FACTORY(text_def);
		virtual ~text_def();

		void read(bit_stream_ptr bits);

		void set_render_type(unsigned rt) { render_type_ = rt; }
		void set_grid_fit(unsigned gf) { grid_fit_ = gf; }
		void set_thickness(float thickness) { thickness_ = thickness; }
		void set_sharpness(float sharpness) { sharpness_ = sharpness; }

		virtual bool is_a(ASClass id) override  { return id == ASClass::TEXT_DEF ? true : character_def::is_a(id); }
		character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id);
	private:
		text_def();
		rect text_bounds_;
		matrix2x3 text_matrix_;

		uint16_t font_id_;
		int x_offset_;
		int y_offset_;
		rgba color_;
		int text_height_;

		unsigned render_type_;
		unsigned grid_fit_;
		float thickness_;
		float sharpness_;

		std::vector<unsigned> glyph_indices_;
		std::vector<int> glyph_advance_;

		text_def(const text_def&) = delete;
		void operator=(const text_def&) = delete;
	};

	typedef std::shared_ptr<text_def> text_def_ptr;

	class text : public character
	{
	public:
		text(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::TEXT ? true : character::is_a(id); }
	private:
		void handle_draw() const override;
	};
}
