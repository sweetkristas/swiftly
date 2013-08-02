#pragma once

#include "bit_reader.hpp"
#include "swf.hpp"

namespace swf
{
	class text : public character
	{
	public:
		text();
		virtual ~text();

		void read(bit_stream_ptr bits);

		void set_render_type(unsigned rt) { render_type_ = rt; }
		void set_grid_fit(unsigned gf) { grid_fit_ = gf; }
		void set_thickness(float thickness) { thickness_ = thickness; }
		void set_sharpness(float sharpness) { sharpness_ = sharpness; }

		void draw() const;
	private:
		geometry::rect text_bounds_;
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

		text(const text&);
	};

	typedef boost::intrusive_ptr<text> text_ptr;
}
