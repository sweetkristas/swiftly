#include "swf_action.hpp"
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

	character_ptr button_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		return std::make_shared<button>(player, parent, id, shared_from_this());
	}

	void button_def::read2(bit_stream_ptr bits)
	{
		bits->read_unsigned_bits(7); // reserved
		track_as_menu_button_ = bits->read_unsigned_bits(1) ? true : false;
		uint16_t action_offset = bits->read_unsigned16();
		button_records_ = read_button_records(2, bits);
		// Would read buttoncondactions here if action_offset != 0
		std::vector<button_conditional_action> cond_actions_;
		if(action_offset != 0) {
			button_conditional_action ca;
			while(ca.read(bits)) {
				cond_actions_.emplace_back(ca);
			}
		}
	}

	button::button(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: character(player, parent, id, def)
	{
	}

	void button::handle_draw() const
	{
		LOG_DEBUG("drawing button: " << get_id());
	}

	button_conditional_action::button_conditional_action()
		: cond_idle_to_over_down_(false),
		  cond_out_down_to_idle_(false),
		  cond_out_down_to_over_down_(false),
		  cond_over_down_to_out_down_(false),
		  cond_over_down_to_over_up_(false),
		  cond_over_up_over_down_(false),
		  cond_over_up_to_idle_(false),
		  cond_idle_to_over_up_(false),
		  cond_over_down_to_idle_(false),
		  key_code_(0)
	{
	}

	bool button_conditional_action::read(bit_stream_ptr bits)
	{
		uint16_t offset_to_next_record = bits->read_unsigned16();
		cond_idle_to_over_down_ = bits->read_unsigned_bits(1) != 0;
		cond_out_down_to_idle_ = bits->read_unsigned_bits(1) != 0;
		cond_out_down_to_over_down_ = bits->read_unsigned_bits(1) != 0;
		cond_over_down_to_out_down_ = bits->read_unsigned_bits(1) != 0;
		cond_over_down_to_over_up_ = bits->read_unsigned_bits(1) != 0;
		cond_over_up_over_down_ = bits->read_unsigned_bits(1) != 0;
		cond_over_up_to_idle_ = bits->read_unsigned_bits(1) != 0;
		cond_idle_to_over_up_ = bits->read_unsigned_bits(1) != 0;
		key_code_ = bits->read_unsigned_bits(7);
		cond_over_down_to_idle_ = bits->read_unsigned_bits(1) != 0;
		actions_ = action::create(bits);
		return offset_to_next_record != 0;
	}
}
