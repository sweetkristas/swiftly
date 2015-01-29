#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "geometry.hpp"
#include "swf_types.hpp"

namespace swf
{
	class bit_stream : public std::enable_shared_from_this<bit_stream>
	{
	public:
		explicit bit_stream(const std::vector<uint8_t>& data);
		virtual ~bit_stream();

		size_t size();

		void force_byte_align() { last_read_bits_remaining_ = 0; }

		// Some commands need to know this information
		int swf_version() const { return swf_version_; }
		void set_swf_version(int version) { swf_version_ = version; }

		int get_bits_read() const { return used_bits_.size() * 8; }
		
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

		rect read_rect();

		matrix2x3 read_matrix();

		color_transform read_cxform();
		color_transform read_cxform_with_alpha();

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