#pragma once

#include "decompress.hpp"
#include "SDL_image.h"
#include "swf.hpp"

namespace swf
{
	class image_character : public character
	{
	public:
		image_character(int version, int block_length) 
			: version_(version), block_length_(block_length), 
			deblock_(0), data_size_(0) {
			if(version == 2) {
				data_size_ = block_length;
			}
		}
		virtual ~image_character() {
		}
		void read(bit_stream_ptr bits) {
			if(version_ == 3 || version_ == 4) {
				data_size_ = bits->read_unsigned32();
			}
			if(version_ == 4) {
				deblock_ = bits->read_unsigned16();
			}
			std::vector<uint8_t> buff = bits->read_unsigned8(data_size_);
			SDL_RWops* rw = SDL_RWFromConstMem(&buff[0], data_size_);
			image_ = std::shared_ptr<SDL_Surface>(IMG_Load_RW(rw, true), [](SDL_Surface* img){SDL_FreeSurface(img);});

			if(version_ == 3 || version_ == 4) {
				// Read alpha data
				int alpha_data_length = block_length_ - 6 - data_size_;
				alpha_ = zip::decompress(bits->read_unsigned8(alpha_data_length));
			}
		}
		void read_lossless(bit_stream_ptr bits) {
			uint8_t format = bits->read_unsigned8();
			uint16_t width = bits->read_unsigned16();
			uint16_t height = bits->read_unsigned16();
			int color_table_length = version_ == 1 ? 3 : 4;
			if(format == 3) {
				size_t color_table_size = bits->read_unsigned8() + 1;
				std::vector<SDL_Color> palette;
				// read colormapdata
				std::vector<uint8_t> imgdata = zip::decompress(bits->read_unsigned8(block_length_ - 6));
				pixels_.assign(&imgdata[color_table_size*color_table_length], &imgdata[imgdata.size()-1]);
				palette.resize(color_table_size);
				for(size_t n = 0; n != color_table_size; ++n) {
					palette[n].r = imgdata[n*color_table_length+0];
					palette[n].g = imgdata[n*color_table_length+1];
					palette[n].b = imgdata[n*color_table_length+2];
					palette[n].a = version_ == 1 ? 0 : imgdata[n*color_table_length+3];
				}
				image_.reset(SDL_CreateRGBSurfaceFrom(&imgdata[color_table_size*color_table_length],
					width, height, 8, ((width + 3) & -4), 0, 0, 0, 0), 
					[](SDL_Surface* img){SDL_FreeSurface(img);});
				ASSERT_LOG(image_->format != NULL && image_->format->palette != NULL, "No palette allocated.");
				SDL_SetPaletteColors(image_->format->palette, &palette[0], 0, color_table_size);
			} else if(format == 4) {
				// read bitmapdata
				pixels_ = zip::decompress(bits->read_unsigned8(block_length_ - 5));
				image_.reset(SDL_CreateRGBSurfaceFrom(&pixels_[0],
					width, height, 16, ((width + 3) & -4), 0x7c00, 0x03e0, 0x001f, 0), 
					[](SDL_Surface* img){SDL_FreeSurface(img);});
			} else if(format == 5) {
				// read bitmapdata
				pixels_ = zip::decompress(bits->read_unsigned8(block_length_ - 5));
				image_.reset(SDL_CreateRGBSurfaceFrom(&pixels_[0],
					width, height, 24, 0, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000), 
					[](SDL_Surface* img){SDL_FreeSurface(img);});
			} else {
				ASSERT_LOG(false, "Unexpected lossless format: " << format);;
			}
		}
		void draw() const {
		}
	private:
		int version_;
		int block_length_;
		uint16_t deblock_;
		uint32_t data_size_;
		std::shared_ptr<SDL_Surface> image_;
		std::vector<uint8_t> pixels_;
		std::vector<uint8_t> alpha_;

		image_character(const image_character&);
	};
}
