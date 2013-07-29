#include "asserts.hpp"
#include "bit_reader.hpp"

namespace swf
{
	matrix2x3::matrix2x3()
		: scale_x(1,0), scale_y(1,0),
		translate_x(0), translate_y(0)
	{}

	fixed_point::fixed_point() : high(0), low(0)
	{}

	fixed_point::fixed_point(int hi, int lo) : high(hi), low(lo)
	{}

	double fixed_point::to_double()
	{
		return double(high) + double(low/65536.0);
	}

	fixed_point8::fixed_point8() : high(0), low(0)
	{}

	fixed_point8::fixed_point8(int hi, int lo) : high(hi), low(lo)
	{}

	float fixed_point8::to_float()
	{
		return float(high) + float(low/256.0f);
	}

	color_transform::color_transform(bool has_alpha)
		: transform_alpha(has_alpha)
	{}

	rgba::rgba() : r(0), g(0), b(0), a(255)
	{}

	bit_stream::bit_stream(const std::vector<uint8_t>& data)
		: last_read_bits_remaining_(0), last_read_(0)
	{
		std::copy(data.begin(), data.end(), std::back_inserter(bitstream_));
	}

	size_t bit_stream::size()
	{
		return bitstream_.size() + used_bits_.size();
	}

	bit_stream::~bit_stream()
	{}
		
	int32_t bit_stream::read_signed_bits(size_t bits)
	{
		uint32_t result = uint32_t(read_bits(bits));
		if(result & (1 << (bits-1))) {
			result |= 0xffffffff & ~((1 << (bits-1))-1);
		}
		return *reinterpret_cast<int32_t*>(&result);
	}

	uint32_t bit_stream::read_unsigned_bits(size_t bits)
	{
		return uint32_t(read_bits(bits));
	}

	fixed_point bit_stream::read_fixedpoint_bits(size_t bits)
	{
		uint32_t result = uint32_t(read_bits(bits));
		fixed_point fp;
		if(bits > 16) {
			fp.high = result >> (bits-16);
		}
		fp.low = result & 0xffff;
		return fp;
	}

	fixed_point8 bit_stream::read_fixedpoint8_bits(size_t bits)
	{
		uint32_t result = uint32_t(read_bits(bits));
		fixed_point8 fp;
		if(bits > 8) {
			fp.high = result >> (bits-8);
		}
		fp.low = result & 0xff;
		return fp;
	}

	fixed_point8 bit_stream::read_fixedpoint8()
	{
		return read_fixedpoint8_bits(16);
	}

	std::string bit_stream::read_string()
	{
		std::string s;
		uint8_t c = read_signed8();
		while(c != 0) {
			s += c;
			c = read_signed8();
		}
		return s;
	}

	int8_t bit_stream::read_signed8()
	{
		ASSERT_LOG(bitstream_.size() != 0, "Tried to read bytes when none available.");
		int8_t result = int8_t(bitstream_.front());
		last_read_bits_remaining_ = 0;
		bitstream_.pop_front();
		used_bits_.push_back(result);
		return result;
	}

	int16_t bit_stream::read_signed16()
	{
		int16_t lsb = int16_t(read_signed8());
		int16_t msb = int16_t(read_signed8());
		return (msb << 8) | lsb;
	}

	int32_t bit_stream::read_signed32()
	{
		int32_t lsb = int32_t(read_signed16());
		int32_t msb = int32_t(read_signed16());
		return (msb << 8) | lsb;
	}

	std::vector<int8_t> bit_stream::read_signed8(size_t n)
	{
		std::vector<int8_t> v;
		for(int i = 0; i != n; ++i) {
			v.push_back(read_signed8());
		}
		return v;
	}

	std::vector<int16_t> bit_stream::read_signed16(size_t n)
	{
		std::vector<int16_t> v;
		for(int i = 0; i != n; ++i) {
			v.push_back(read_signed16());
		}
		return v;
	}

	uint8_t bit_stream::read_unsigned8()
	{
		ASSERT_LOG(bitstream_.size() != 0, "Tried to read bytes when none available.");
		uint8_t result = bitstream_.front();
		last_read_bits_remaining_ = 0;
		bitstream_.pop_front();
		used_bits_.push_back(result);
		return result;
	}

	uint16_t bit_stream::read_unsigned16()
	{
		uint16_t lsb = uint16_t(read_unsigned8());
		uint16_t msb = uint16_t(read_unsigned8());
		return (msb << 8) | lsb;
	}

	uint32_t bit_stream::read_unsigned32()
	{
		uint32_t lsb = uint32_t(read_unsigned16());
		uint32_t msb = uint32_t(read_unsigned16());
		return (msb << 16) | lsb;
	}

	std::vector<uint8_t> bit_stream::read_unsigned8(size_t n)
	{
		std::vector<uint8_t> v;
		for(int i = 0; i != n; ++i) {
			v.push_back(read_unsigned8());
		}
		return v;
	}

	std::vector<uint16_t> bit_stream::read_unsigned16(size_t n)
	{
		std::vector<uint16_t> v;
		for(int i = 0; i != n; ++i) {
			v.push_back(read_unsigned16());
		}
		return v;
	}

	std::vector<uint32_t> bit_stream::read_unsigned24(size_t n)
	{
		std::vector<uint32_t> v;
		for(int i = 0; i != n; ++i) {
			uint32_t lsb = uint32_t(read_unsigned16());
			uint32_t msb = uint32_t(read_unsigned8());
			v.push_back((msb << 16) | lsb);
		}
		return v;
	}

	std::vector<uint32_t> bit_stream::read_unsigned32(size_t n)
	{
		std::vector<uint32_t> v;
		for(int i = 0; i != n; ++i) {
			v.push_back(read_unsigned32());
		}
		return v;
	}

	std::vector<uint64_t> bit_stream::read_unsigned64(size_t n)
	{
		std::vector<uint64_t> v;
		for(int i = 0; i != n; ++i) {
			uint64_t lsb = uint64_t(read_unsigned32());
			uint64_t msb = uint64_t(read_unsigned32());
			v.push_back((msb << 32) | lsb);
		}
		return v;
	}

	float bit_stream::read_half_float()
	{
		uint16_t uval = read_unsigned16();
		bool negative = uval & (1<<15) ? true : false;
		unsigned exponent = (uval & 0x83ff) >> 10;
		unsigned mantissa = (uval & 0x03ff);
		if(exponent != 0) {
			return (negative ? -1.0f : 1.0f) * pow(2.0f, exponent - 16.0f) * (1.0f + mantissa/2048.0f);
		} else if(exponent == 32) {
			// NaN
			ASSERT_LOG(false, "read_half_float: NaN");
		}
		return (negative ? -1.0f : 1.0f) * pow(2.0f, -14.0f) * (0.0f + mantissa/2048.0f);
	}

	float bit_stream::read_float()
	{
		uint32_t uval = read_unsigned32();
		return *reinterpret_cast<float*>(&uval);
	}

	double bit_stream::read_double()
	{
		uint64_t lsw = uint64_t(read_unsigned32());
		uint64_t msw = uint64_t(read_unsigned32());
		uint64_t uval = (msw << 32) | lsw;
		return *reinterpret_cast<double*>(&uval);
	}

	uint32_t bit_stream::read_unsigned32_encoded()
	{
		uint32_t result = read_unsigned8();
		uint32_t mask = 0x80;
		int shift = 7;
		while((result & mask) != 0 && shift < 28) {
			result = (result & mask-1) | read_unsigned8() << shift;
			mask <<= 0x80;
			shift += 7;
		}
		return result;
	}

	rgb bit_stream::read_rgb()
	{
		rgb color;
		color.r = read_unsigned8();
		color.g = read_unsigned8();
		color.b = read_unsigned8();
		return color;
	}

	rgba bit_stream::read_rgba()
	{
		rgba color;
		color.r = read_unsigned8();
		color.g = read_unsigned8();
		color.b = read_unsigned8();
		color.a = read_unsigned8();
		return color;
	}

	rgba bit_stream::read_argb()
	{
		rgba color;
		color.a = read_unsigned8();
		color.r = read_unsigned8();
		color.g = read_unsigned8();
		color.b = read_unsigned8();
		return color;
	}

	rect bit_stream::read_rect()
	{
		uint32_t nbits = read_unsigned_bits(5);
		rect r;
		r.x1 = read_signed_bits(nbits);
		r.x2 = read_signed_bits(nbits);
		r.y1 = read_signed_bits(nbits);
		r.y2 = read_signed_bits(nbits);
		return r;
	}

	matrix2x3 bit_stream::read_matrix()
	{
		matrix2x3 mat;
		uint32_t has_scale = read_unsigned_bits(1);
		if(has_scale) {
			uint32_t scale_bits = read_unsigned_bits(5);
			mat.scale_x = read_fixedpoint_bits(scale_bits);
			mat.scale_y = read_fixedpoint_bits(scale_bits);
		}
		uint32_t has_rotate = read_unsigned_bits(1);
		if(has_rotate) {
			uint32_t rotate_bits = read_unsigned_bits(5);
			mat.rotate_skew0 = read_fixedpoint_bits(rotate_bits);
			mat.rotate_skew1 = read_fixedpoint_bits(rotate_bits);
		}
		uint32_t translate_bits = read_unsigned_bits(5);
		mat.translate_x = read_signed_bits(translate_bits);
		mat.translate_x = read_signed_bits(translate_bits);
		return mat;
	}

	color_transform bit_stream::read_cxform()
	{
		color_transform ct;
		uint32_t has_add_values = read_unsigned_bits(1);
		uint32_t has_mul_values = read_unsigned_bits(1);
		uint32_t nbits = read_unsigned_bits(4);
		if(has_mul_values) {
			// XXX 
			ct.mult.r = read_fixedpoint8_bits(nbits);
			ct.mult.g = read_fixedpoint8_bits(nbits);
			ct.mult.b = read_fixedpoint8_bits(nbits);
		}
		if(has_add_values) {
			ct.add.r = read_signed_bits(nbits);
			ct.add.g = read_signed_bits(nbits);
			ct.add.b = read_signed_bits(nbits);
		}
		return ct;
	}

	color_transform bit_stream::read_cxform_with_alpha()
	{
		color_transform ct(true);
		uint32_t has_add_values = read_unsigned_bits(1);
		uint32_t has_mul_values = read_unsigned_bits(1);
		uint32_t nbits = read_unsigned_bits(4);
		if(has_mul_values) {
			// XXX 
			ct.mult.r = read_fixedpoint8_bits(nbits);
			ct.mult.g = read_fixedpoint8_bits(nbits);
			ct.mult.b = read_fixedpoint8_bits(nbits);
			ct.mult.a = read_fixedpoint8_bits(nbits);
		}
		if(has_add_values) {
			ct.add.r = read_signed_bits(nbits);
			ct.add.g = read_signed_bits(nbits);
			ct.add.b = read_signed_bits(nbits);
			ct.add.a = read_signed_bits(nbits);
		}
		return ct;
	}

	gradient_record bit_stream::read_gradient_record(int version)
	{
		// shape3 reads rgba not rgb.
		gradient_record gr;
		gr.ratio = unsigned(read_unsigned8());
		if(version == 3) {
			gr.color = read_rgba();
		} else {
			rgb color = read_rgb();
			gr.color.r = color.r;
			gr.color.g = color.g;
			gr.color.b = color.b;
			gr.color.a = 255;
		}
		return gr;		
	}

	gradient bit_stream::read_gradient(int version)
	{
		gradient g;
		g.spread = gradient::spread_mode(read_bits(2));
		g.interpolation = gradient::interpolation_mode(read_bits(2));
		unsigned num_gradients = unsigned(read_bits(4));
		for(unsigned n = 0; n != num_gradients; ++n) {
			g.gradient_records.push_back(read_gradient_record(version));
		}
		return g;
	}

	focal_gradient bit_stream::read_focal_gradient(int version)
	{
		focal_gradient fg;
		fg.g = read_gradient(version);
		fg.focal_point = read_fixedpoint8().to_float();
		return fg;
	}

	fill_style bit_stream::read_fillstyle(int version)
	{
		fill_style fs;
		fs.type = (fill_style::fill_style_type)read_unsigned8();
		ASSERT_LOG(fs.type == fill_style::FILL_SOLID 
			|| fs.type == fill_style::FILL_FOCAL_RADIAL_GRADIENT 
			|| fs.type == fill_style::FILL_LINEAR_GRADIENT 
			|| fs.type == fill_style::FILL_RADIAL_GRADIENT
			|| fs.type == fill_style::FILL_REPEATING_BITMAP
			|| fs.type == fill_style::FILL_CLIPPED_BITMAP
			|| fs.type == fill_style::FILL_NON_SMOOTHED_REPEATING_BITMAP
			|| fs.type == fill_style::FILL_NON_SMOOTHED_CLIPPED_BITMAP,
			"fill style type unrecognised: " << fs.type);
		if(fs.type == fill_style::FILL_SOLID) {
			// shape1&2 read rgb, shape 3 reads rgba
			if(version == 3) {
				fs.color = read_rgba();
			} else {
				rgb color = read_rgb();
				fs.color.r = color.r;
				fs.color.g = color.g;
				fs.color.b = color.b;
				fs.color.a = 255;
			}
		}
		if(fs.type == fill_style::FILL_FOCAL_RADIAL_GRADIENT 
			|| fs.type == fill_style::FILL_LINEAR_GRADIENT 
			|| fs.type == fill_style::FILL_RADIAL_GRADIENT) {
			fs.gradient_matrix = read_matrix();
			if(fs.type == fill_style::FILL_FOCAL_RADIAL_GRADIENT) {
				fs.fg = read_focal_gradient(version);
			} else {
				fs.g = read_gradient(version);
			}
		}
		if(fs.type == fill_style::FILL_REPEATING_BITMAP
			|| fs.type == fill_style::FILL_CLIPPED_BITMAP
			|| fs.type == fill_style::FILL_NON_SMOOTHED_REPEATING_BITMAP
			|| fs.type == fill_style::FILL_NON_SMOOTHED_CLIPPED_BITMAP) {
			fs.bitmap_id = read_unsigned16();
			fs.bitmap_matrix = read_matrix();
		}
		return fs;
	}

	std::vector<fill_style> bit_stream::read_fillstyle_array(int version)
	{
		std::vector<fill_style> fs;
		unsigned count = read_unsigned8();
		if(count == 255) {
			count = read_unsigned16();
		}
		for(int n = 0; n != count; ++n) {
			fs.push_back(read_fillstyle(version));
		}
		return fs;
	}

	line_style bit_stream::read_linestyle(int version)
	{
		line_style ls;
		ls.width = read_unsigned16();
		if(version == 3) {
			ls.color = read_rgba();
		} else {
			rgb color = read_rgb();
			ls.color.r = color.r;
			ls.color.g = color.g;
			ls.color.b = color.b;
			ls.color.a = 255;
		}
		return ls;
	}

	std::vector<line_style> bit_stream::read_linestyle_array(int version)
	{
		std::vector<line_style> ls;
		unsigned count = read_unsigned8();
		if(count == 255) {
			count = read_unsigned16();
		}
		for(int n = 0; n != count; ++n) {
			ls.push_back(read_linestyle(version));
		}
		return ls;
	}

	shape_styles bit_stream::read_style(int version)
	{
		shape_styles ss;
		ss.fill_styles_ = read_fillstyle_array(version);
		ss.line_styles_ = read_linestyle_array(version);
		ss.fill_bits_ = read_unsigned_bits(4);
		ss.line_bits_ = read_unsigned_bits(4);
		return ss;
	}

	shape_record_ptr bit_stream::shape_record_factory(int version, unsigned& fill_bits, unsigned& line_bits)
	{
		bool edge_record = read_bits(1) ? true : false;
		unsigned flags = unsigned(read_bits(5));
		if(flags == 0) {
			return shape_record_ptr();
		}
		if(edge_record) {
			if(flags & 16) {
				// straight
				shape_record* srp = new shape_record;
				unsigned num_bits = (flags & 0x0f) + 2;
				bool general_line = read_bits(1) ? true : false;
				delta_record dr;
				if(general_line) {
					dr.delta_x = int32_t(read_bits(num_bits));
					dr.delta_y = int32_t(read_bits(num_bits));
				} else {
					bool vertical_line = read_bits(1) ? true : false;
					if(vertical_line) {
						dr.delta_x = int32_t(read_bits(num_bits));
						dr.delta_y = 0;
					} else {
						dr.delta_x = 0;
						dr.delta_y = int32_t(read_bits(num_bits));
					}
				}
				srp->set_delta(dr);
				return shape_record_ptr(srp);
			} else {
				curve_edge_record* cer = new curve_edge_record;
				unsigned num_bits = (flags & 0x0f) + 2;
				delta_record dr;
				dr.delta_x = int32_t(read_bits(num_bits));
				dr.delta_y = int32_t(read_bits(num_bits));
				cer->set_control(dr);
				dr.delta_x = int32_t(read_bits(num_bits));
				dr.delta_y = int32_t(read_bits(num_bits));
				cer->set_anchor(dr);
				return shape_record_ptr(cer);
			}
		} else {
			style_change_record* srp = new style_change_record;
			if(flags & 1) {
				delta_record moves;
				unsigned move_bits = unsigned(read_bits(5));
				moves.delta_x = int32_t(read_signed_bits(move_bits));
				moves.delta_y = int32_t(read_signed_bits(move_bits));
				srp->set_moves(moves);
			}
			if(flags & 2) {
				srp->set_fillstyle0_index(read_unsigned_bits(fill_bits));
			}
			if(flags & 4) {
				srp->set_fillstyle1_index(read_unsigned_bits(fill_bits));
			}
			if(flags & 8) {
				srp->set_linestyle_index(read_unsigned_bits(line_bits));
			}
			if(flags & 16) {
				shape_styles ss = read_style(version);
				fill_bits = ss.fill_bits_;
				line_bits = ss.line_bits_;
				srp->set_styles(ss);
			}
			return shape_record_ptr(srp);
		}
	}

	std::vector<shape_record_ptr> bit_stream::read_shape_records(int version, unsigned fill_bits, unsigned line_bits)
	{
		std::vector<shape_record_ptr> sr;
		bool done = false;
		while(!done) {
			shape_record_ptr p = shape_record_factory(version, fill_bits, line_bits);
			if(p) {
				sr.push_back(p);
			} else {
				done = true;
			}
		}
		return sr;
	}

	shape_with_style bit_stream::read_shape_with_style(int version)
	{
		shape_with_style ss;
		ss.style_ = read_style(version);
		ss.shape_records_ = read_shape_records(version, ss.style_.fill_bits_, ss.style_.line_bits_);
		return ss;
	}

	uint64_t bit_stream::read_bits(size_t n)
	{
		ASSERT_LOG(n <= 64, "Can only read a maximum of 64-bits at once.");
		
		uint64_t result = 0;
		while(n > 0) {
			if(last_read_bits_remaining_ == 0) {
				ASSERT_LOG(bitstream_.size() != 0, "Tried to read bytes when none available.");
				last_read_ = bitstream_.front();
				bitstream_.pop_front();
				used_bits_.push_back(last_read_);
				last_read_bits_remaining_ = 8;
			}
			if(n < last_read_bits_remaining_) {
				result <<= n;
				last_read_bits_remaining_ -= n;
				result |= last_read_ >> (8-n);
				last_read_ <<= n;
				n = 0;
			} else {
				result = (result << last_read_bits_remaining_) | (last_read_ >> (8-last_read_bits_remaining_));
				n -= last_read_bits_remaining_;
				last_read_bits_remaining_ = 0;
			}
		}
		return result;
	}

}
