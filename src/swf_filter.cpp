#include "asserts.hpp"
#include "swf_filter.hpp"

namespace swf
{
	filter_ptr filter_factory(bit_stream_ptr bits)
	{
		unsigned filter_type = bits->read_unsigned8();
		switch(filter_type) {
			case 0: return new drop_shadow_filter(bits);
			case 1: return new blur_filter(bits);
			case 2: return new glow_filter(bits);
			case 3: return new bevel_filter(bits);
			case 4: return new gradient_glow_filter(bits);
			case 5: return new convolution_filter(bits);
			case 6: return new color_matrix_filter(bits);
			case 7: return new gradient_bevel_filter(bits);
			default:
				ASSERT_LOG(false, "Unrecognised filter type: " << filter_type);
		}
		return filter_ptr();
	}

	filter_list read_filter_list(bit_stream_ptr bits)
	{
		filter_list fl;
		unsigned num_filters = bits->read_unsigned8();
		for(unsigned n = 0; n != num_filters; ++n) {
			fl.push_back(filter_factory(bits));
		}
		return fl;
	}

	convolution_filter::convolution_filter(bit_stream_ptr bits)
	{
		mat_x_size_ = bits->read_unsigned8();
		mat_y_size_ = bits->read_unsigned8();
		bias_ = bits->read_float();
		divisor_ = bits->read_float();
		matrix_.resize(mat_x_size_);
		for(int x = 0; x != mat_x_size_; ++x) {
			matrix_[x].resize(mat_y_size_);
			for(int y = 0; y != mat_y_size_; ++y) {
				matrix_[x][y] = bits->read_float();
			}
		}
		default_color_ = bits->read_rgba();
		bits->read_unsigned_bits(6); // reserved
		clamp_ = bits->read_unsigned_bits(1) ? true : false;
		preserve_alpha_ = bits->read_unsigned_bits(1) ? true : false;
	}

	color_matrix_filter::color_matrix_filter(bit_stream_ptr bits)
	{
		for(int n = 0; n != 20; ++n) {
			values_[n] = bits->read_float();
		}
	}

	drop_shadow_filter::drop_shadow_filter(bit_stream_ptr bits)
	{
		color_ = bits->read_rgba();
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		angle_ = bits->read_fixedpoint().to_double();
		distance_ = bits->read_fixedpoint().to_double();
		strength_ = bits->read_fixedpoint8().to_float();
		inner_shadow_ = bits->read_unsigned_bits(1) ? true : false;
		knockout_ = bits->read_unsigned_bits(1) ? true : false;
		bits->read_unsigned_bits(1); // composite source, always reads 1
		passes_ = bits->read_unsigned_bits(5);
	}

	blur_filter::blur_filter(bit_stream_ptr bits)
	{
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		passes_ = bits->read_unsigned_bits(5);
		bits->read_unsigned_bits(3); // reserved
	}

	glow_filter::glow_filter(bit_stream_ptr bits)
	{
		color_ = bits->read_rgba();
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		strength_ = bits->read_fixedpoint8().to_float();
		inner_glow_ = bits->read_unsigned_bits(1) ? true : false;
		knockout_ = bits->read_unsigned_bits(1) ? true : false;
		bits->read_unsigned_bits(1); // composite source, always reads 1
		passes_ = bits->read_unsigned_bits(5);
	}

	bevel_filter::bevel_filter(bit_stream_ptr bits)
	{
		shadow_color_ = bits->read_rgba();
		highlight_color_ = bits->read_rgba();
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		angle_ = bits->read_fixedpoint().to_double();
		distance_ = bits->read_fixedpoint().to_double();
		strength_ = bits->read_fixedpoint8().to_float();
		inner_shadow_ = bits->read_unsigned_bits(1) ? true : false;
		knockout_ = bits->read_unsigned_bits(1) ? true : false;
		bits->read_unsigned_bits(1); // composite source, always reads 1
		ontop_ = bits->read_unsigned_bits(1) ? true : false;
		passes_ = bits->read_unsigned_bits(4);
	}

	gradient_glow_filter::gradient_glow_filter(bit_stream_ptr bits)
	{
		unsigned num_gradient_colors = bits->read_unsigned8();
		for(unsigned n = 0; n != num_gradient_colors; ++n) {
			gradient_colors_.push_back(bits->read_rgba());
		}
		for(unsigned n = 0; n != num_gradient_colors; ++n) {
			gradient_raitos_.push_back(bits->read_unsigned8());
		}
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		angle_ = bits->read_fixedpoint().to_double();
		distance_ = bits->read_fixedpoint().to_double();
		strength_ = bits->read_fixedpoint8().to_float();
		inner_shadow_ = bits->read_unsigned_bits(1) ? true : false;
		knockout_ = bits->read_unsigned_bits(1) ? true : false;
		bits->read_unsigned_bits(1); // composite source, always reads 1
		ontop_ = bits->read_unsigned_bits(1) ? true : false;
		passes_ = bits->read_unsigned_bits(4);
	}

	gradient_bevel_filter::gradient_bevel_filter(bit_stream_ptr bits)
	{
		unsigned num_gradient_colors = bits->read_unsigned8();
		for(unsigned n = 0; n != num_gradient_colors; ++n) {
			gradient_colors_.push_back(bits->read_rgba());
		}
		for(unsigned n = 0; n != num_gradient_colors; ++n) {
			gradient_raitos_.push_back(bits->read_unsigned8());
		}
		blur_x_ = bits->read_fixedpoint().to_double();
		blur_y_ = bits->read_fixedpoint().to_double();
		angle_ = bits->read_fixedpoint().to_double();
		distance_ = bits->read_fixedpoint().to_double();
		strength_ = bits->read_fixedpoint8().to_float();
		inner_shadow_ = bits->read_unsigned_bits(1) ? true : false;
		knockout_ = bits->read_unsigned_bits(1) ? true : false;
		bits->read_unsigned_bits(1); // composite source, always reads 1
		ontop_ = bits->read_unsigned_bits(1) ? true : false;
		passes_ = bits->read_unsigned_bits(4);
	}

}
