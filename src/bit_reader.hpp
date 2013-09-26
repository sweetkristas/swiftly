#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "polygon.hpp"

namespace swf
{
	struct fixed_point
	{
		fixed_point();
		explicit fixed_point(int hi, int lo);
		double to_double();
		int16_t high;
		uint16_t low;
	};

	struct fixed_point8
	{
		fixed_point8();
		explicit fixed_point8(int hi, int lo);
		float to_float();
		int8_t high;
		uint8_t low;
	};

	struct rgb
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	struct rgba
	{
		rgba();
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct matrix2x3
	{
		matrix2x3();
		fixed_point scale_x;
		fixed_point scale_y;
		fixed_point rotate_skew0;
		fixed_point rotate_skew1;
		int32_t translate_x;
		int32_t translate_y;
	};

	struct color_transform
	{
		explicit color_transform(bool has_alpha=false);
		struct {
			fixed_point8 r;
			fixed_point8 g;
			fixed_point8 b;
			fixed_point8 a;
		} mult;
		struct {
			int8_t r;
			int8_t g;
			int8_t b;
			int8_t a;
		} add;
		bool transform_alpha;
	};

	struct gradient_record
	{
		unsigned ratio;
		rgba color;
	};

	struct gradient
	{
		enum spread_mode {
			SPREAD_PAD,
			SPREAD_REFLECT,
			SPREAD_REPEAT,
		} spread;
		enum interpolation_mode {
			INTERPOLATE_NORMAL_RGB,
			INTERPOLATE_LINEAR_RGB,
		} interpolation;
		std::vector<gradient_record> gradient_records;
	};

	struct focal_gradient
	{
		gradient g;
		float focal_point;
	};

	struct fill_style
	{
		enum fill_style_type {
			FILL_SOLID,
			FILL_LINEAR_GRADIENT = 16,
			FILL_RADIAL_GRADIENT = 18,
			FILL_FOCAL_RADIAL_GRADIENT = 19,
			FILL_REPEATING_BITMAP = 64,
			FILL_CLIPPED_BITMAP,
			FILL_NON_SMOOTHED_REPEATING_BITMAP,
			FILL_NON_SMOOTHED_CLIPPED_BITMAP,
		} type;
		rgba color;
		matrix2x3 gradient_matrix;
		gradient g;
		focal_gradient fg;
		unsigned bitmap_id;
		matrix2x3 bitmap_matrix;
	};

	struct line_style
	{
		unsigned width;
		rgba color;

		enum {
			CAP_STYLE_ROUND,
			CAP_STYLE_NONE,
			CAP_STYLE_SQUARE,
		};
		enum {
			JOIN_STYLE_ROUND,
			JOIN_STYLE_BEVEL,
			JOIN_STYLE_MITER,
		};
		// linestyle2 features
		bool is_linestyle2;
		unsigned start_cap_style;
		unsigned join_style;
		bool has_fill_flag;
		bool no_h_scale;
		bool no_v_scale;
		bool pixel_hinting;
		bool no_close;
		unsigned end_cap_style;
		float miter_limit_factor; // only if join_style = 2
		fill_style fs; // only use if has_fill_flag=1, else use color.
	};

	struct delta_record
	{
		delta_record() : delta_x(0), delta_y(0) {}
		int32_t delta_x;
		int32_t delta_y;
	};

	class action_record
	{
	public:
		enum {
			ACTION_GOTO_FRAME = 0x81,
			ACTION_GET_URL = 0x83,
			ACTION_NEXT_FRAME = 0x04,
			ACTION_PREVIOUS_FRAME = 0x05,
			ACTION_PLAY = 0x06,
			ACTION_STOP = 0x07,
			ACTION_TOGGLE_QUALITY = 0x08,
			ACTION_STOP_SOUNDS = 0x09,
			ACTION_WAIT_FOR_FRAME = 0x8a,
			ACTION_SET_TARGET = 0x8b,
			ACTION_GOTO_LABEL = 0x8c,
		};
		action_record() {}
		virtual ~action_record() {}
		uint8_t code() const { return code_; }
		void set_code(uint8_t c) { code_ = c; }
	private:
		uint8_t code_;
	};

	typedef std::shared_ptr<action_record> action_record_ptr;

	struct clip_event
	{
		bool key_up_;
		bool key_down_;
		bool mouse_up_;
		bool mouse_down_;
		bool mouse_move_;
		bool unload_;
		bool enter_frame_;
		bool load_;
		// swf 6 and greater only
		bool drag_over_;
		bool roll_out;
		bool roll_over_;
		bool release_outside_;
		bool mouse_release_;
		bool mouse_press_;
		bool initialise_;
		bool data_received_;
		// SWF >= 7, SWF = 6, then 0
		bool construct_;
		bool key_press_;
		bool drag_out_;
	};

	struct clip_action_record
	{
		clip_event flags;
		uint8_t key_code;
		std::vector<action_record_ptr> actions;
	};

	struct clip_actions
	{
		clip_event flags;
		std::vector<clip_action_record> clip_records;
	};

	class bit_stream
	{
	public:
		explicit bit_stream(const std::vector<uint8_t>& data);
		virtual ~bit_stream();

		size_t size();

		void force_byte_align() { last_read_bits_remaining_ = 0; }

		// Some commands need to know this information
		int swf_version() const { return swf_version_; }
		void set_swf_version(int version) { swf_version_ = version; }
		
		int32_t read_signed_bits(size_t bits);
		uint32_t read_unsigned_bits(size_t bits);
		fixed_point read_fixedpoint_bits(size_t bits);
		fixed_point read_fixedpoint();
		fixed_point8 read_fixedpoint8_bits(size_t bits);
		fixed_point8 read_fixedpoint8();
		
		std::string read_string();
		std::string read_avm2_string();

		int8_t read_signed8();
		int16_t read_signed16();
		int32_t read_signed32();
		std::vector<int8_t> read_signed8(size_t n);
		std::vector<int16_t> read_signed16(size_t n);

		uint8_t read_unsigned8();
		uint16_t read_unsigned16();
		uint32_t read_unsigned32();

		uint8_t bit_stream::peek_unsigned8();

		uint32_t read_u30();
		uint32_t read_u32();
		int32_t read_s24();
		int32_t read_s32();

		std::vector<uint8_t> read_unsigned8(size_t n);
		std::vector<uint16_t> read_unsigned16(size_t n);
		std::vector<uint32_t> read_unsigned24(size_t n);
		std::vector<uint32_t> read_unsigned32(size_t n);
		std::vector<uint64_t> read_unsigned64(size_t n);

		float read_half_float();
		float read_float();
		double read_double();

		uint32_t read_unsigned32_encoded();

		rgb read_rgb();
		rgba read_rgba();
		rgba read_argb();

		geometry::rect read_rect();

		matrix2x3 read_matrix();

		color_transform read_cxform();
		color_transform read_cxform_with_alpha();

		action_record_ptr read_action_record();
		std::vector<action_record_ptr> read_action_records();

		clip_event parse_clip_event_flags(uint32_t flags);
		std::vector<clip_action_record> read_clip_action_records();
		clip_actions read_clip_actions();

		gradient_record read_gradient_record(int version);
		gradient read_gradient(int version);
		focal_gradient read_focal_gradient(int version);
		fill_style read_fillstyle(int version);
		line_style read_linestyle(int version);
		std::vector<fill_style> read_fillstyle_array(int version);
		std::vector<line_style> read_linestyle_array(int version);
	private:
		uint64_t read_bits(size_t n);

		int swf_version_;

		uint8_t last_read_;
		size_t last_read_bits_remaining_;
		std::list<uint8_t> bitstream_;
		std::list<uint8_t> used_bits_;

		bit_stream();
		bit_stream(const bit_stream&);
	};

	typedef std::shared_ptr<bit_stream> bit_stream_ptr;
}