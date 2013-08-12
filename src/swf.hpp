#pragma once

#include <map>

#include "avm2_fwd.hpp"
#include "asserts.hpp"
#include "bit_reader.hpp"
#include "ref_counted_ptr.hpp"

namespace swf
{
	class frame : public reference_counted_ptr
	{
	public:
		frame();
		virtual ~frame();
	private:
		frame(const frame&);
	};
	typedef boost::intrusive_ptr<frame> frame_ptr;

	// Main base class for character that provides simple id support.
	class character : public reference_counted_ptr
	{
	public:
		character() : id_(0) {}
		virtual ~character() {}

		uint16_t id() const { return id_; }
		void set_id(uint16_t id) { id_ = id; }

		virtual void draw() const = 0;
	private:
		uint16_t id_;

		character(const character&);
	};
	typedef boost::intrusive_ptr<character> character_ptr;

	class displayable
	{
	public:
		displayable(character_ptr char_id);
		virtual ~displayable();

		void draw() const;

		const matrix2x3& transform() const { return transform_; }
		const color_transform& cxtransform() const { return ct_; }
		int clip_depth() const { return clip_depth_; }
		float ratio() const { return ratio_; }

		void set_transform(const matrix2x3& t) { transform_ = t; }
		void set_cxtransform(const color_transform& ct) { ct_ = ct; }
		void set_name(const std::string& name) { name_ = name; }
		void set_clip_depth(int cd) { clip_depth_ = cd; }
		void set_ratio(uint16_t r) { ratio_ = r/65536.0f; }
	private:
		character_ptr character_;
		matrix2x3 transform_;
		color_transform ct_;
		std::string name_;
		int clip_depth_;
		float ratio_;

		displayable();
		displayable(const displayable&);
	};
	typedef std::shared_ptr<displayable> displayable_ptr;

	typedef std::map<uint16_t, character_ptr> character_map;

	typedef uint16_t depth_id;
	typedef std::map<depth_id, displayable_ptr> display_list;

	typedef std::vector<std::pair<unsigned,std::string> > scene_info;
	typedef std::vector<std::pair<unsigned,std::string> > frame_label_info;

	class swf
	{
	public:
		swf();
		virtual ~swf();

		unsigned version() const { return version_; }
		void set_version(unsigned vers);
		const geometry::rect& frame_size() const { return frame_size_; }
		void set_frame_size(geometry::rect frame_size);
		double frame_rate() const { return frame_rate_; }
		void set_frame_rate(fixed_point frame_rate);
		size_t frame_count() const { return frames_.size(); }
		void set_frame_count(unsigned frame_count);
		void set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network);
		void set_background_color(const rgb& color) { background_color_ = color; }
		void set_scene_and_frame_info(const scene_info& scene, const frame_label_info& frame_label);
		void set_transparent_background(bool transparency=true) { transparent_background_ = transparency; }

		unsigned twip() const { return 20; }

		void add_character(uint16_t id, character_ptr p) {
			auto it = char_map_.find(id);
			ASSERT_LOG(it == char_map_.end(), "Duplicated character found at id: " << id);
			char_map_[id] = p;
		}

		void remove_character(uint16_t id) {
			auto it = char_map_.find(id);
			ASSERT_LOG(it != char_map_.end(), "No character found with id: " << id);
			char_map_.erase(it);
		}

		character_ptr find_character(uint16_t id) {
			auto it = char_map_.find(id);
			ASSERT_LOG(it != char_map_.end(), "No character found with id: " << id);
			return it->second;
		}

		void add_character_to_display_list(depth_id depth, displayable_ptr d) {
			auto dl_it = display_list_.find(depth);
			ASSERT_LOG(dl_it == display_list_.end(), "character already at depth " << depth);
			display_list_[depth] = d;
		}

		void remove_character_from_display_list(depth_id depth) { 
			auto dl_it = display_list_.find(depth);
			ASSERT_LOG(dl_it != display_list_.end(), "No character at depth " << depth);
			display_list_.erase(dl_it);
		}

		displayable_ptr find_displayable(depth_id depth) {
			auto dl_it = display_list_.find(depth);
			ASSERT_LOG(dl_it != display_list_.end(), "No character at depth " << depth);
			return dl_it->second;
		}

		void add_abc(std::shared_ptr<avm2::abc_file> abc) {
			abcs_.push_back(abc);
		}

		void add_symbol_class(uint16_t tag, const std::string& name) {
			exported_symbols_.push_back(std::make_pair(tag,name));
		}

		void draw() const;
	private:
		int version_;
		geometry::rect frame_size_;
		double frame_rate_;

		// file attributes
		bool use_direct_blit_;
		bool use_gpu_;
		bool use_as3_;
		bool use_network_;

		// Special override to make a transparent background.
		bool transparent_background_;

		rgb background_color_;

		scene_info scene_info_;
		frame_label_info frame_label_;

		std::vector<frame_ptr> frames_;

		character_map char_map_;

		display_list display_list_;

		std::vector<avm2::abc_file_ptr> abcs_;

		std::vector<std::pair<uint16_t,std::string> > exported_symbols_;

		swf(const swf&);
	};
}
