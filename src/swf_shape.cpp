#include "swf_shape.hpp"

namespace swf 
{
	shape_def::shape_def()
	{
	}

	shape_def::~shape_def()
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

	void shape_def::read(bit_stream_ptr bits)
	{
		// plain shape, no styles
		style_.fill_bits_ = bits->read_unsigned_bits(4);
		style_.line_bits_ = bits->read_unsigned_bits(4);
		shape_records_ = read_shape_records(bits, 0, style_.fill_bits_, style_.line_bits_);
	}

	void shape_def::read(int version, bit_stream_ptr bits)
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
			

	void shape_def::draw() const
	{
	}

	void shape_def::prepare_for_draw()
	{
		bool in_shape = false;
		for(auto sr : shape_records_) {
			if(sr->type() == shape_record::SR_STYLE_CHANGE) {
				std::shared_ptr<style_change_record> p = std::dynamic_pointer_cast<style_change_record>(sr);
				if(p->has_newstyles()) {
					// XXX
				}
				if(p->has_fillstyle0_change()) {
				}
				if(p->has_fillstyle1_change()) {
				}
				if(p->has_linestyle_change()) {
				}
				if(p->has_moves()) {
					if(in_shape) {
						// XXX
						// test whether the last vertices forms an enclosed space.
					}
					current_x_ = p->get_delta().delta_x;
					current_y_ = p->get_delta().delta_y;
				}
			} else if(sr->type() == shape_record::SR_LINE) {
				// line from current_x_/current_y_ to sr->get_delta().x/.y
			} else if(sr->type() == shape_record::SR_CURVE) {
				// curve from current_x_/current_y_ to sr->anchor().x/y with sr->control().x/y
			}
		}
	}
}
