#include "swf_shape.hpp"

namespace swf 
{
	shape::shape()
	{
	}

	shape::~shape()
	{
	}

	shape_record_ptr shape_record_factory(bit_stream_ptr bits, int version, unsigned& fill_bits, unsigned& line_bits)
	{
		bool edge_record = bits->read_unsigned_bits(1) ? true : false;
		unsigned flags = unsigned(bits->read_unsigned_bits(5));
		if(flags == 0) {
			return shape_record_ptr();
		}
		if(edge_record) {
			if(flags & 16) {
				// straight
				shape_record* srp = new shape_record;
				unsigned num_bits = (flags & 0x0f) + 2;
				bool general_line = bits->read_unsigned_bits(1) ? true : false;
				delta_record dr;
				if(general_line) {
					dr.delta_x = int32_t(bits->read_unsigned_bits(num_bits));
					dr.delta_y = int32_t(bits->read_unsigned_bits(num_bits));
				} else {
					bool vertical_line = bits->read_unsigned_bits(1) ? true : false;
					if(vertical_line) {
						dr.delta_x = int32_t(bits->read_signed_bits(num_bits));
						dr.delta_y = 0;
					} else {
						dr.delta_x = 0;
						dr.delta_y = int32_t(bits->read_signed_bits(num_bits));
					}
				}
				srp->set_delta(dr);
				return shape_record_ptr(srp);
			} else {
				curve_edge_record* cer = new curve_edge_record;
				unsigned num_bits = (flags & 0x0f) + 2;
				delta_record dr;
				dr.delta_x = int32_t(bits->read_signed_bits(num_bits));
				dr.delta_y = int32_t(bits->read_signed_bits(num_bits));
				cer->set_control(dr);
				dr.delta_x = int32_t(bits->read_signed_bits(num_bits));
				dr.delta_y = int32_t(bits->read_signed_bits(num_bits));
				cer->set_anchor(dr);
				return shape_record_ptr(cer);
			}
		} else {
			style_change_record* srp = new style_change_record;
			if(flags & 1) {
				delta_record moves;
				unsigned move_bits = unsigned(bits->read_unsigned_bits(5));
				moves.delta_x = int32_t(bits->read_signed_bits(move_bits));
				moves.delta_y = int32_t(bits->read_signed_bits(move_bits));
				srp->set_moves(moves);
			}
			if(flags & 2) {
				srp->set_fillstyle0_index(bits->read_unsigned_bits(fill_bits));
			}
			if(flags & 4) {
				srp->set_fillstyle1_index(bits->read_unsigned_bits(fill_bits));
			}
			if(flags & 8) {
				srp->set_linestyle_index(bits->read_unsigned_bits(line_bits));
			}
			if(flags & 16) {
				styles ss;
				ss.fill_styles_ = bits->read_fillstyle_array(version);
				ss.line_styles_ = bits->read_linestyle_array(version);
				fill_bits = ss.fill_bits_ = bits->read_unsigned_bits(4);
				line_bits = ss.line_bits_ = bits->read_unsigned_bits(4);
				srp->set_styles(ss);
			}
			return shape_record_ptr(srp);
		}
	}

	std::vector<shape_record_ptr> read_shape_records(bit_stream_ptr bits, int version, unsigned fill_bits, unsigned line_bits)
	{
		std::vector<shape_record_ptr> sr;
		bool done = false;
		while(!done) {
			shape_record_ptr p = shape_record_factory(bits, version, fill_bits, line_bits);
			if(p) {
				sr.push_back(p);
			} else {
				done = true;
			}
		}
		return sr;
	}

	void shape::read(bit_stream_ptr bits)
	{
		// plain shape, no styles
		style_.fill_bits_ = bits->read_unsigned_bits(4);
		style_.line_bits_ = bits->read_unsigned_bits(4);
		shape_records_ = read_shape_records(bits, 0, style_.fill_bits_, style_.line_bits_);
	}

	void shape::read(int version, bit_stream_ptr bits)
	{
		bounds_ = bits->read_rect();
		if(version == 4) {
			edge_bounds_ = bits->read_rect();
			bits->read_unsigned_bits(5); // reserved
			uses_fill_winding_rule_ = bits->read_unsigned_bits(1) ? true : false;
			has_non_scaling_strokes_ = bits->read_unsigned_bits(1) ? true : false;
			has_scaling_strokes_ = bits->read_unsigned_bits(1) ? true : false;
		}
		style_.fill_styles_ = bits->read_fillstyle_array(version);
		style_.line_styles_ = bits->read_linestyle_array(version);
		style_.fill_bits_ = bits->read_unsigned_bits(4);
		style_.line_bits_ = bits->read_unsigned_bits(4);
		shape_records_ = read_shape_records(bits, version, style_.fill_bits_, style_.line_bits_);
	}
			

	void shape::draw() const
	{
		for(auto sr : shape_records_) {
			sr->draw(const_cast<shape&>(*this));
		}
	}

	void style_change_record::handle_draw(shape& shp) const 
	{ 
		if(has_new_styles_) {
			shp.set_current_fillstyle_array(styles_.fill_styles_);
			shp.set_current_linestyle_array(styles_.line_styles_);
		}
		if(has_fs0_) {
			shp.set_current_fillstyle0(fillstyle0_);
		}
		if(has_fs1_) {
			shp.set_current_fillstyle1(fillstyle1_);
		}
		if(has_ls_) {
			shp.set_current_linestyle(linestyle_);
		}
		if(has_moves_) {
			shp.move_to(get_delta().delta_x, get_delta().delta_y);
		}
	}
}
