#pragma once

#include "bit_reader.hpp"

namespace swf
{
	class filter
	{
	public:
		filter() {}
		virtual ~filter() {}

	private:
		filter(const filter&);
	};

	typedef std::shared_ptr<filter> filter_ptr;

	class convolution_filter : public filter
	{
	public:
		convolution_filter(bit_stream_ptr bits);
		virtual ~convolution_filter()
		{}

	private:
		unsigned mat_x_size_;
		unsigned mat_y_size_;
		float bias_;
		float divisor_;
		std::vector<std::vector<float> > matrix_;
		rgba default_color_;
		bool clamp_;
		bool preserve_alpha_;

		convolution_filter();
		convolution_filter(const convolution_filter&);
	};

	typedef std::shared_ptr<convolution_filter> convolution_filter_ptr;

	class color_matrix_filter : public filter
	{
	public:
		color_matrix_filter(bit_stream_ptr bits);
		virtual ~color_matrix_filter()
		{}

	private:
		float values_[20];

		color_matrix_filter();
		color_matrix_filter(const color_matrix_filter&);
	};

	typedef std::shared_ptr<color_matrix_filter> color_matrix_filter_ptr;

	class drop_shadow_filter : public filter
	{
	public:
		drop_shadow_filter(bit_stream_ptr bits);
		virtual ~drop_shadow_filter()
		{}

	private:
		rgba color_;
		double blur_x_;
		double blur_y_;
		double angle_;
		double distance_;
		float strength_;
		bool inner_shadow_;
		bool knockout_;
		unsigned passes_;

		drop_shadow_filter();
		drop_shadow_filter(const drop_shadow_filter&);
	};

	typedef std::shared_ptr<drop_shadow_filter> drop_shadow_filter_ptr;

	class blur_filter : public filter
	{
	public:
		blur_filter(bit_stream_ptr bits);
		virtual ~blur_filter()
		{}

	private:
		double blur_x_;
		double blur_y_;
		unsigned passes_;

		blur_filter();
		blur_filter(const blur_filter&);
	};

	typedef std::shared_ptr<blur_filter> blur_filter_ptr;

	class glow_filter : public filter
	{
	public:
		glow_filter(bit_stream_ptr bits);
		virtual ~glow_filter()
		{}

	private:
		rgba color_;
		double blur_x_;
		double blur_y_;
		float strength_;
		bool inner_glow_;
		bool knockout_;
		unsigned passes_;

		glow_filter();
		glow_filter(const glow_filter&);
	};

	typedef std::shared_ptr<glow_filter> glow_filter_ptr;

	class bevel_filter : public filter
	{
	public:
		bevel_filter(bit_stream_ptr bits);
		virtual ~bevel_filter()
		{}

	private:
		rgba shadow_color_;
		rgba highlight_color_;
		double blur_x_;
		double blur_y_;
		double angle_;
		double distance_;
		float strength_;
		bool inner_shadow_;
		bool knockout_;
		bool ontop_;
		unsigned passes_;

		bevel_filter();
		bevel_filter(const bevel_filter&);
	};

	typedef std::shared_ptr<bevel_filter> bevel_filter_ptr;

	class gradient_glow_filter : public filter
	{
	public:
		gradient_glow_filter(bit_stream_ptr bits);
		virtual ~gradient_glow_filter()
		{}

	private:
		std::vector<rgba> gradient_colors_;
		std::vector<uint8_t> gradient_raitos_;
		double blur_x_;
		double blur_y_;
		double angle_;
		double distance_;
		float strength_;
		bool inner_shadow_;
		bool knockout_;
		bool ontop_;
		unsigned passes_;

		gradient_glow_filter();
		gradient_glow_filter(const gradient_glow_filter&);
	};

	typedef std::shared_ptr<gradient_glow_filter> gradient_glow_filter_ptr;

	class gradient_bevel_filter : public filter
	{
	public:
		gradient_bevel_filter(bit_stream_ptr bits);
		virtual ~gradient_bevel_filter()
		{}

	private:
		std::vector<rgba> gradient_colors_;
		std::vector<uint8_t> gradient_raitos_;
		double blur_x_;
		double blur_y_;
		double angle_;
		double distance_;
		float strength_;
		bool inner_shadow_;
		bool knockout_;
		bool ontop_;
		unsigned passes_;

		gradient_bevel_filter();
		gradient_bevel_filter(const gradient_bevel_filter&);
	};

	typedef std::shared_ptr<gradient_bevel_filter> gradient_bevel_filter_ptr;

	filter_ptr filter_factory(bit_stream_ptr bits);

	typedef std::vector<filter_ptr> filter_list;
	filter_list read_filter_list(bit_stream_ptr bits);
}
