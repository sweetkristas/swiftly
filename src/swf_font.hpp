#pragma once

#include <map>
#include "bit_reader.hpp"
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

	class font_def : public character_def
	{
	public:
		MAKE_FACTORY(font_def);
		virtual ~font_def();

		virtual bool is_a(ASClass id) override  { return id == ASClass::FONT_DEF ? true : character_def::is_a(id); }

		void read(int vers, bit_stream_ptr bits);

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
	private:
		font_def();
		void read1(bit_stream_ptr bits);

		uint8_t language_code_;
		std::vector<uint16_t> code_points_;
		uint16_t font_ascender_height_;
		uint16_t font_descender_height_;
		int16_t font_leading_height_;
		std::vector<int16_t> font_advance_table_;
		std::vector<rect> font_bounds_table_;
		kerning_map kerning_records_;
		std::vector<shape_def_ptr> glyphs_;
		std::string font_name_;
		unsigned csm_hint_;
		std::vector<zone_record> zone_records_;

		font_def(const font_def&) = delete;
		void operator=(const font_def&) = delete;
	};

	typedef std::shared_ptr<font_def> font_def_ptr;

	class font : public character
	{
	public:
		font(player_ptr player, const character_ptr& parent, int id, const font_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::FONT ? true : character::is_a(id); }
	private:
	};
}
