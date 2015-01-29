#pragma once

#include "bit_reader.hpp"
#include "swf_character.hpp"
#include "swf_types.hpp"
#include "swf_player.hpp"

namespace swf
{
	class edit_text_def : public character_def
	{
	public:
		MAKE_FACTORY(text_def);
		
		void read(bit_stream_ptr bits);
		virtual bool is_a(ASClass id) override  { return id == ASClass::EDIT_TEXT_DEF ? true : character_def::is_a(id); }
	private:
		edit_text_def();
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
		text(player_ptr player, const character_ptr& parent, int id, const text_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::TEXT ? true : character::is_a(id); }
	private:
	};
}
