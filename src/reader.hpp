#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace swf
{
	struct fixed_point
	{
		fixed_point();
		explicit fixed_point(int hi, int lo);
		int16_t high;
		uint16_t low;
	};

	struct fixed_point8
	{
		fixed_point8();
		explicit fixed_point8(int hi, int lo);
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

	class bit_stream
	{
	public:
		explicit bit_stream(const std::vector<uint8_t>& data);
		virtual ~bit_stream();
		
		int32_t read_signed_bits(size_t bits);
		uint32_t read_unsigned_bits(size_t bits);
		fixed_point read_fixedpoint_bits(size_t bits);
		fixed_point8 read_fixedpoint8_bits(size_t bits);
		
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
	private:
		uint64_t read_bits(size_t n);

		uint8_t last_read_;
		size_t last_read_bits_remaining_;
		std::list<uint8_t> bitstream_;
		std::list<uint8_t> used_bits_;

		bit_stream();
		bit_stream(const bit_stream&);
	};
}