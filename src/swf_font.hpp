#pragma once

#include <map>
#include "bit_reader.hpp"
#include "swf.hpp"
#include "swf_shape.hpp"

namespace swf
{
	struct zone_record
	{
		struct {
			float alignment_coordinate;
			float range;
		} zone_data[2];
		bool zone_mask_y;
		bool zone_mask_x;
	};

	typedef std::pair<uint16_t,uint16_t> glyph_pair;
	typedef std::map<glyph_pair, int16_t> kerning_map;

	class font : public character
	{
	public:
		font();
		virtual ~font();

		void read3(bit_stream_ptr bits);

		size_t size() const { return glyphs_.size(); }

		enum {
			CSM_THIN,
			CSM_MEDIUM,
			CSM_THICK,
		};
		void set_csm_hint(unsigned hint) { csm_hint_ = hint; }
		void set_zone_records(std::vector<zone_record>* zone_records) { 
			zone_records_.swap(*zone_records); 
			ASSERT_LOG(zone_records_.size() == glyphs_.size(), "Zone records size != glyphs size : " << zone_records_.size() << " != " << glyphs_.size());
		}

		void draw() const;
	private:
		uint8_t language_code_;
		std::vector<uint16_t> code_points_;
		uint16_t font_ascender_height_;
		uint16_t font_descender_height_;
		int16_t font_leading_height_;
		std::vector<int16_t> font_advance_table_;
		std::vector<rect> font_bounds_table_;
		kerning_map kerning_records_;
		std::vector<shape_ptr> glyphs_;
		std::string font_name_;
		unsigned csm_hint_;
		std::vector<zone_record> zone_records_;

		font(const font&);
	};

	typedef boost::intrusive_ptr<font> font_ptr;
}
