#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

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

	struct rect
	{
		size_t x1;
		size_t y1;
		size_t x2;
		size_t y2;
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
	};

	struct delta_record
	{
		delta_record() : delta_x(0), delta_y(0) {}
		int32_t delta_x;
		int32_t delta_y;
	};

	class shape_record
	{
	public:
		shape_record() {}
		virtual ~shape_record() {}

		void set_delta(const delta_record& delta) { delta_ = delta; }

		void draw() { handle_draw(); }
	protected:
		virtual void handle_draw() { std::cerr << "Drew a straight edge" << std::endl; }
	private:
		delta_record delta_;
	};

	typedef std::shared_ptr<shape_record> shape_record_ptr;

	struct shape_styles
	{
		std::vector<fill_style> fill_styles_;
		std::vector<line_style> line_styles_;
		unsigned fill_bits_;
		unsigned line_bits_;
	};

	class style_change_record : public shape_record
	{
	public:
		style_change_record() : shape_record() {}
		virtual ~style_change_record() {}
		void set_moves(const delta_record& moves) { moves_.reset(new delta_record(moves)); }
		void set_fillstyle0_index(uint32_t fs) { fillstyle0_.reset(new uint32_t); *fillstyle0_ = fs; }
		void set_fillstyle1_index(uint32_t fs) { fillstyle1_.reset(new uint32_t); *fillstyle1_ = fs; }
		void set_linestyle_index(uint32_t ls) { linestyle_.reset(new uint32_t); *linestyle_ = ls; }
		void set_styles(const shape_styles& styles) { styles_.reset(new shape_styles(styles)); }
	protected:
		void handle_draw() { std::cerr << "Changed styles" << std::endl; }
	private:
		std::unique_ptr<delta_record> moves_;
		std::unique_ptr<uint32_t> fillstyle0_;
		std::unique_ptr<uint32_t> fillstyle1_;
		std::unique_ptr<uint32_t> linestyle_;
		std::unique_ptr<shape_styles> styles_;

		style_change_record(const style_change_record&);
	};

	class curve_edge_record : public shape_record
	{
	public:
		curve_edge_record() : shape_record() {}
		virtual ~curve_edge_record() {}

		void set_anchor(const delta_record& delta) { anchor_ = delta; }
		void set_control(const delta_record& delta) { control_ = delta; }
	protected:
		void handle_draw() { std::cerr << "Drew a curved edge" << std::endl; }
	private:
		delta_record control_;
		delta_record anchor_;

		curve_edge_record(const curve_edge_record&);
	};

	shape_record_ptr shape_record_factory();

	struct shape_with_style
	{
		shape_styles style_;
		std::vector<shape_record_ptr> shape_records_;
	};

	struct shape
	{
		unsigned fill_bits_;
		unsigned line_bits_;
		std::vector<shape_record_ptr> shape_records_;
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
		fixed_point8 read_fixedpoint8_bits(size_t bits);
		fixed_point8 read_fixedpoint8();
		
		std::string read_string();

		int8_t read_signed8();
		int16_t read_signed16();
		int32_t read_signed32();
		std::vector<int8_t> read_signed8(size_t n);
		std::vector<int16_t> read_signed16(size_t n);

		uint8_t read_unsigned8();
		uint16_t read_unsigned16();
		uint32_t read_unsigned32();

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

		rect read_rect();

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
		shape_styles read_style(int version);
		fill_style read_fillstyle(int version);
		line_style read_linestyle(int version);
		std::vector<fill_style> read_fillstyle_array(int version);
		std::vector<line_style> read_linestyle_array(int version);
		shape_record_ptr shape_record_factory(int version, unsigned& fill_bits, unsigned& line_bits);
		std::vector<shape_record_ptr> read_shape_records(int version, unsigned fill_bits, unsigned line_bits);
		shape_with_style read_shape_with_style(int version);
		shape read_shape();
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
}