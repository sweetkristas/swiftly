#pragma once

#include "bit_reader.hpp"
#include "swf_character.hpp"

namespace swf 
{
	class shape;

	typedef std::vector<fill_style> fill_style_array;
	typedef std::shared_ptr<fill_style_array> fill_style_array_ptr;
	typedef std::vector<line_style> line_style_array;
	typedef std::shared_ptr<line_style_array> line_style_array_ptr;

	struct styles
	{
		std::vector<fill_style> fill_styles_;
		std::vector<line_style> line_styles_;
		unsigned fill_bits_;
		unsigned line_bits_;
	};

	enum class ShapeRecordTypes {
		STYLE_CHANGE,
		MOVE,
		LINE,
		CURVE,
	};

	class shape_record
	{
	public:
		explicit shape_record(ShapeRecordTypes type) : type_(type) {}
		virtual ~shape_record() {}

		void set_delta(const delta_record& delta) { delta_ = delta; }
		const delta_record& get_delta() const { return delta_; }

		ShapeRecordTypes get_type() const { return type_; }
		void set_type(ShapeRecordTypes t) { type_ = t; }
	private:
		delta_record delta_;
		ShapeRecordTypes type_;
	};

	typedef std::shared_ptr<shape_record> shape_record_ptr;

	class move_record : public shape_record
	{
	public:
		move_record() : shape_record(ShapeRecordTypes::MOVE) {}
	private:
		move_record(const move_record&);
	};

	class style_change_record : public shape_record
	{
	public:
		style_change_record() 
			: shape_record(ShapeRecordTypes::STYLE_CHANGE), 
			  has_fs0_(false), 
			  has_fs1_(false), 
			  has_ls_(false), 
			  has_new_styles_(false),
			  fillstyle0_(0), 
			  fillstyle1_(0), 
			  linestyle_(0)
		{
		}
		void set_fillstyle0_index(uint32_t fs) { has_fs0_ = true; fillstyle0_ = fs; }
		void set_fillstyle1_index(uint32_t fs) { has_fs1_ = true; fillstyle1_ = fs; }
		void set_linestyle_index(uint32_t ls) { has_ls_ = true; linestyle_ = ls; }
		void set_styles(const styles& s) { has_new_styles_ = true; styles_ = s; }

		int get_line_style() const { return linestyle_; }

		bool has_linestyle_change() const { return has_ls_; }
		bool has_fillstyle0_change() const { return has_fs0_; }
		bool has_fillstyle1_change() const { return has_fs1_; }
		bool has_newstyles() const { return has_new_styles_; }
	private:
		bool has_fs0_;
		bool has_fs1_;
		bool has_ls_;
		bool has_new_styles_;

		unsigned fillstyle0_;
		unsigned fillstyle1_;
		unsigned linestyle_;
		styles styles_;

		style_change_record(const style_change_record&);
	};

	class curve_edge_record : public shape_record
	{
	public:
		curve_edge_record() : shape_record(ShapeRecordTypes::CURVE) {}
		void set_anchor(const delta_record& delta) { anchor_ = delta; }
		void set_control(const delta_record& delta) { control_ = delta; }

		const delta_record& anchor() const { return anchor_; }
		const delta_record& control() const { return control_; }
	private:
		delta_record control_;
		delta_record anchor_;

		curve_edge_record(const curve_edge_record&);
	};

	std::vector<shape_record_ptr> read_shape_records(bit_stream_ptr bits, int version, unsigned fill_bits, unsigned line_bits);
	shape_record_ptr shape_record_factory(bit_stream_ptr bits, int version, unsigned& fill_bits, unsigned& line_bits);

	class shape_def : public character_def
	{
	public:
		MAKE_FACTORY(shape_def);

		shape_def();
		virtual ~shape_def();

		virtual bool is_a(ASClass id) override { return id == ASClass::SHAPE_DEF ? true : character_def::is_a(id); }

		void read(bit_stream_ptr bits);
		void read(int version, bit_stream_ptr bits);

		void set_current_styles(int fs0, int fs1, int ls) {
			current_fill_style0_ = fs0;
			current_fill_style1_ = fs1;
			current_line_style_ = ls;
		}
		void set_current_fillstyle0(int fs0) {
			current_fill_style0_ = fs0;
		}
		void set_current_fillstyle1(int fs1) {
			current_fill_style1_ = fs1;
		}
		void set_current_linestyle(int ls) {
			current_line_style_ = ls;
		}
		void set_current_fillstyle_array(const fill_style_array& fs_ary) {
			current_fill_style_array_.reset(new fill_style_array(fs_ary));
		}
		void set_current_linestyle_array(const line_style_array& ls_ary) {
			current_line_style_array_.reset(new line_style_array(ls_ary));
		}
		const fill_style* get_fill_style0() const {
			if(current_fill_style0_ == 0) {
				return NULL;
			} else {
				return &(*current_fill_style_array_)[current_fill_style0_-1];
			}
		}
		const fill_style* get_fill_style1() const {
			if(current_fill_style1_ == 0) {
				return NULL;
			} else {
				return &(*current_fill_style_array_)[current_fill_style1_-1];
			}
		}
		const line_style* get_line_style(int line_style) const {
			if(line_style == 0) {
				return nullptr;
			} else {
				return &(*current_line_style_array_)[line_style-1];
			}
		}
		const void move_to(int32_t x, int32_t y) {
			current_x_ = x;
			current_y_ = y;
		}
		const void line_to(int32_t x, int32_t y) {
			current_x_ = x;
			current_y_ = y;
		}

		virtual character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id);
		virtual void update(float delta_time, character_ptr instance);
	private:
		virtual void handle_draw(const matrix2x3& mat, const color_transform& ct, const character_ptr& instance) const override;

		styles style_;
		rect bounds_;
		std::vector<shape_record_ptr> shape_records_;

		int current_fill_style0_;
		int current_fill_style1_;
		int current_line_style_;
		fill_style_array_ptr current_fill_style_array_;
		line_style_array_ptr current_line_style_array_;

		int32_t current_x_;
		int32_t current_y_;

		// Stuff for DefineShape4
		rect edge_bounds_;
		bool uses_fill_winding_rule_;
		bool has_non_scaling_strokes_;
		bool has_scaling_strokes_;

		shape_def(const shape_def&) = delete;
		void operator=(const shape_def&) = delete;
	};

	typedef std::shared_ptr<shape_def> shape_def_ptr;

	class shape : public character
	{
	public:
		shape(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::SHAPE ? true : character::is_a(id); }
	private:
	};
}
