#include "kre/VGraph.hpp"

#include "swf_character.hpp"
#include "swf_player.hpp"
#include "swf_shape.hpp"

namespace swf 
{
	shape_def::shape_def()
	{
	}

	shape_def::~shape_def()
	{
	}

	bool shape_record_factory(bit_stream_ptr bits, int version, unsigned& fill_bits, unsigned& line_bits, std::vector<shape_record_ptr>* records)
	{
		bool edge_record = bits->read_unsigned_bits(1) ? true : false;
		unsigned flags = unsigned(bits->read_unsigned_bits(5));
		if(flags == 0) {
			bits->force_byte_align();
			return false;
		}
		if(edge_record) {
			if(flags & 16) {
				// straight
				auto srp = std::make_shared<shape_record>(ShapeRecordTypes::LINE);
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
				records->emplace_back(srp);
				return true;
			} else {
				auto cer = std::make_shared<curve_edge_record>();
				unsigned num_bits = (flags & 0x0f) + 2;
				delta_record dr;
				dr.delta_x = int32_t(bits->read_signed_bits(num_bits));
				dr.delta_y = int32_t(bits->read_signed_bits(num_bits));
				cer->set_control(dr);
				dr.delta_x = int32_t(bits->read_signed_bits(num_bits));
				dr.delta_y = int32_t(bits->read_signed_bits(num_bits));
				cer->set_anchor(dr);
				records->emplace_back(cer);
				return true;
			}
		} else {
			if(flags & 1) {
				std::shared_ptr<move_record> mrp = std::make_shared<move_record>();
				delta_record moves;
				unsigned move_bits = unsigned(bits->read_unsigned_bits(5));
				moves.delta_x = int32_t(bits->read_signed_bits(move_bits));
				moves.delta_y = int32_t(bits->read_signed_bits(move_bits));
				mrp->set_delta(moves);
				records->emplace_back(mrp);
			}
			if(flags & (2|4|8|16)) {
				std::shared_ptr<style_change_record> srp = std::make_shared<style_change_record>();
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
				records->emplace_back(srp);
			}
			return true;
		}
	}

	std::vector<shape_record_ptr> read_shape_records(bit_stream_ptr bits, int version, unsigned fill_bits, unsigned line_bits)
	{
		std::vector<shape_record_ptr> sr;
		while(shape_record_factory(bits, version, fill_bits, line_bits, &sr)) {
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

	void shape_def::update(float delta_time, character_ptr instance)
	{
		// XXX optimize this.
		auto vgraph = std::dynamic_pointer_cast<KRE::Vector::Context>(instance->get_render_object());
		auto path = vgraph->NewPath();
		for(auto& sr : shape_records_) {
			switch(sr->get_type())
			{
			case ShapeRecordTypes::MOVE:
				path->MoveTo(sr->get_delta().delta_x, sr->get_delta().delta_y);
				break;
			case ShapeRecordTypes::LINE:
				path->LineTo(sr->get_delta().delta_x, sr->get_delta().delta_y);
				break;
			case ShapeRecordTypes::CURVE: {
				auto cr = std::dynamic_pointer_cast<curve_edge_record>(sr);
				ASSERT_LOG(cr != nullptr, "Type was curve. cast was null.");
				path->QuadraticCurveTo(cr->control().delta_x, cr->control().delta_y, 
					cr->anchor().delta_x+cr->control().delta_x, cr->anchor().delta_y+cr->control().delta_y, 
					true);
				break;
			}
			case ShapeRecordTypes::STYLE_CHANGE: {
				auto scr = std::dynamic_pointer_cast<style_change_record>(sr);
				if(scr->has_linestyle_change()) {
					auto ls = get_line_style(scr->get_line_style());
					// XXX if ls->end_cap_style != ls->start_cap_style we need to split the line.
					// or something.
					switch(ls->end_cap_style) {
					case line_style::CAP_STYLE_NONE: 
						vgraph->SetLineCap(KRE::Vector::LineCap::LINE_CAP_BUTT);
						break;
					case line_style::CAP_STYLE_SQUARE:
						vgraph->SetLineCap(KRE::Vector::LineCap::LINE_CAP_SQUARE);
						break;
					case line_style::CAP_STYLE_ROUND:
						vgraph->SetLineCap(KRE::Vector::LineCap::LINE_CAP_ROUND);
						break;
					default: break;
					}
					vgraph->SetLineWidth(static_cast<double>(ls->width) / instance->get_player()->get_twips());
					switch(ls->join_style) {
					case line_style::JOIN_STYLE_BEVEL:
						vgraph->SetLineJoin(KRE::Vector::LineJoin::LINE_JOIN_BEVEL);
						break;
					case line_style::JOIN_STYLE_MITER:
						vgraph->SetLineJoin(KRE::Vector::LineJoin::LINE_JOIN_MITER);
						vgraph->SetMiterLimit(ls->miter_limit_factor);
						break;
					case line_style::JOIN_STYLE_ROUND:
						vgraph->SetLineJoin(KRE::Vector::LineJoin::LINE_JOIN_ROUND);
						break;
					}
					if(ls->has_fill_flag) {
						// apply fill change
					}
					vgraph->SetSourceColor(ls->color.r, ls->color.g, ls->color.b, ls->color.a);
				}
				break;
			}
			default: break;
			}
		}
		vgraph->AddPath(path);
		vgraph->Stroke(true);
		vgraph->Fill();
	}

	void shape_def::handle_draw(const matrix2x3& mat, const color_transform& ct, const character_ptr& instance) const
	{
	}

	character_ptr shape_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		auto playr = player.lock();
		auto shp = std::make_shared<shape>(player, parent, id, shared_from_this());
		shp->set_render_object(KRE::Vector::Context::CreateInstance("cairo", bounds_.w()/playr->get_twips(), bounds_.h()/playr->get_twips()));
		return shp;
	}

	shape::shape(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: character(player, parent, id, def)
	{
	}
}
