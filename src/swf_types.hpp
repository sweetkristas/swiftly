#pragma once

#include <iostream>

namespace swf
{
	struct fixed_point
	{
		fixed_point();
		explicit fixed_point(int hi, int lo);
		double to_double();
		float to_float();
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
		rgb() : r(0), g(0), b(0) {}
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	inline std::ostream& operator<<(std::ostream& os, const rgb& value)
	{
		os << "rgb(" 
			<< static_cast<int>(value.r) << "," 
			<< static_cast<int>(value.g) << "," 
			<< static_cast<int>(value.b) << ")";
		return os;
	}

	struct rgba
	{
		rgba() : r(0), g(0), b(0), a(255) {}
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	inline std::ostream& operator<<(std::ostream& os, const rgba& value)
	{
		os << "rgba(" 
			<< static_cast<int>(value.r) << "," 
			<< static_cast<int>(value.g) << "," 
			<< static_cast<int>(value.b) << "," 
			<< static_cast<int>(value.a) << ")";
		return os;
	}

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
}
