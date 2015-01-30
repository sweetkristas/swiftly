#pragma once

#include "bit_reader.hpp"
#include "decompress.hpp"
#include "SDL_image.h"
#include "swf_character.hpp"

namespace swf
{
	class image_def : public character_def
	{
	public:
		MAKE_FACTORY(image_def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::IMAGE_DEF ? true : character_def::is_a(id); }

		void read(bit_stream_ptr bits);
		void read_lossless(bit_stream_ptr bits);
		virtual character_ptr create_instance(const weak_player_ptr& player, const character_ptr& parent, int id);
	private:
		image_def(int version, int block_length);

		int version_;
		int block_length_;
		uint16_t deblock_;
		uint32_t data_size_;
		std::shared_ptr<SDL_Surface> image_;
		std::vector<uint8_t> pixels_;
		std::vector<uint8_t> alpha_;

		image_def(const image_def&) = delete;
		void operator=(const image_def&) = delete;
	};

	typedef std::shared_ptr<image_def> image_def_ptr;

	class image : public character
	{
	public:
		image(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def);
		virtual bool is_a(ASClass id) override  { return id == ASClass::IMAGE ? true : character::is_a(id); }
	private:
		void handle_draw() const override;
	};
}
