#pragma once

#include "swf_types.hpp"

namespace swf
{
	class placement_params
	{
	public:
		placement_params(int depth) 
			: depth_(depth), 
			  has_id_(false), 
			  id_(-1), 
			  has_transform_matrix_(false),
			  m_(),
			  has_color_transform_(false),
			  ct_(),
			  has_morph_ratio_(false),
			  morph_ratio_(0),
			  has_clip_depth_(false),
			  clip_depth_(0),
			  has_name_(false),
			  has_clip_actions_(false)
		{
		}
		int get_depth() const { return depth_; }
		bool has_id() const { return has_id_; }
		int get_id() const { return id_; }
		void set_id(int id) { id_ = id; has_id_ = true; }
		bool has_matrix_transform() const { return has_transform_matrix_; }
		void set_matrix_transform(const matrix2x3& mat) { m_ = mat; has_transform_matrix_ = true; }
		const matrix2x3& get_matrix_transform() const { return m_; }
		bool has_color_transform() const { return has_color_transform_; }
		const color_transform& get_color_transform() const { return ct_; }
		void set_color_transform(const color_transform& ct) { ct_ = ct; has_color_transform_ = true; }
		bool has_morph_ratio() const { return has_morph_ratio_; }
		int get_morph_ratio() const { return morph_ratio_; }
		void set_morph_ratio(int mr) { morph_ratio_ = mr; has_morph_ratio_ = true; }
		bool has_clip_depth() const { return has_clip_depth_; }
		int get_clip_depth() const { return clip_depth_; }
		void set_clip_depth(int cd) { clip_depth_ = cd; has_clip_depth_ = true; }
		bool has_name() const { return has_name_; }
		const std::string& get_name() const { return name_; }
		void set_name(const std::string& name) { name_ = name; has_name_ = true; }
		bool has_clip_actions() const { return has_clip_actions_; }
		const clip_actions& get_clip_actions() const { return actions_; }
		void set_clip_actions(const clip_actions& ca) { actions_ = ca; has_clip_actions_ = true; }
	private:
		int depth_; 
		bool has_id_;
		int id_; 
		bool has_transform_matrix_;
		matrix2x3 m_; 
		bool has_color_transform_;
		color_transform ct_;
		bool has_morph_ratio_;
		int morph_ratio_;
		bool has_clip_depth_;
		int clip_depth_;
		bool has_name_;
		std::string name_;
		bool has_clip_actions_;
		clip_actions actions_;
	};
}
