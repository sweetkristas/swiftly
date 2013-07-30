#pragma once

#include <map>

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

	typedef std::map<uint16_t, character_ptr> character_map;

	typedef std::vector<std::pair<unsigned,std::string> > scene_info;
	typedef std::vector<std::pair<unsigned,std::string> > frame_label_info;

	class swf
	{
	public:
		swf();
		virtual ~swf();

		unsigned version() const { return version_; }
		void set_version(unsigned vers);
		const rect& frame_size() const { return frame_size_; }
		void set_frame_size(rect frame_size);
		double frame_rate() const { return frame_rate_; }
		void set_frame_rate(fixed_point frame_rate);
		size_t frame_count() const { return frames_.size(); }
		void set_frame_count(unsigned frame_count);
		void set_attributes(bool use_direct_blit, bool use_gpu, bool use_as3, bool use_network);
		void set_background_color(const rgb& color) { background_color_ = color; }
		void set_scene_and_frame_info(const scene_info& scene, const frame_label_info& frame_label);

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

	private:
		int version_;
		rect frame_size_;
		double frame_rate_;

		// file attributes
		bool use_direct_blit_;
		bool use_gpu_;
		bool use_as3_;
		bool use_network_;

		rgb background_color_;

		scene_info scene_info_;
		frame_label_info frame_label_;

		std::vector<frame_ptr> frames_;

		character_map char_map_;

		swf(const swf&);
	};
}
