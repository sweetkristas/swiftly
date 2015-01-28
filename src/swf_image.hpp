#pragma once

#include "bit_reader.hpp"
#include "decompress.hpp"
#include "SDL_image.h"
#include "swf_character.hpp"

namespace swf
{
	class image_character_def : public character_def
	{
	public:
		MAKE_FACTORY(image_character_def);

		void read(bit_stream_ptr bits);
		void read_lossless(bit_stream_ptr bits);
	private:
		image_character_def(int version, int block_length);

		int version_;
		int block_length_;
		uint16_t deblock_;
		uint32_t data_size_;
		std::shared_ptr<SDL_Surface> image_;
		std::vector<uint8_t> pixels_;
		std::vector<uint8_t> alpha_;

		image_character_def(const image_character_def&) = delete;
		void operator=(const image_character_def&) = delete;
	};
}
