#include "swf_button.hpp"
#include "swf_character.hpp"

namespace swf
{
	std::vector<button_record> read_button_records(int version, bit_stream_ptr bits)
	{
		std::vector<button_record> brl;
		while(true) {
			uint8_t flags = bits->read_unsigned8();
			if(flags == 0) {
				return brl;
			}
			button_record br;
			br.character_id_ = bits->read_unsigned16();
			br.depth_ = bits->read_unsigned16();
			br.transform_ = bits->read_matrix();
			if(version >= 2) {
				br.ct_ = bits->read_cxform_with_alpha();
				if(flags & 0x10) {
					br.filter_list_ = read_filter_list(bits);
				}
				if(flags & 0x20) {
					br.blend_mode_ = bits->read_unsigned8();
				}
			br.on_hit_test_ = (flags & 8) ? true : false;
			br.on_down_ = (flags & 4) ? true : false;
			br.on_over_ = (flags & 2) ? true : false;
			br.on_up_ = (flags & 1) ? true : false;
			}

			brl.push_back(br);
		}
	}

	button_def::button_def() 
		: track_as_menu_button_(false)
	{
	}

	void button_def::read2(bit_stream_ptr bits)
	{
		bits->read_unsigned_bits(7); // reserved
		track_as_menu_button_ = bits->read_unsigned_bits(1) ? true : false;
		uint16_t action_offset = bits->read_unsigned16();
		button_records_ = read_button_records(2, bits);
		// Would read buttoncondactions here if action_offset != 0
		if(action_offset != 0) {
			ASSERT_LOG(false, "XXX write code to read button actions.");
		}
	}


	button::button(player_ptr player, const character_ptr& parent, int id, const button_def_ptr& def)
		: character(player, parent, id, def),
		  def_(def)
	{
	}
}
