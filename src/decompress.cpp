#include <zlib.h>
#include "asserts.hpp"
#include "decompress.hpp"


namespace zip
{
	std::vector<uint8_t> decompress(const std::vector<uint8_t>& data)
	{
		return decompress(&data[0], data.size());
	}
	
	std::vector<uint8_t> decompress(const uint8_t* data, size_t length)
	{
		const unsigned int MAX_OUTPUT_SIZE = 256*1024*1024;

		unsigned int output_size = length*10;
		if(output_size > MAX_OUTPUT_SIZE) {
			output_size = MAX_OUTPUT_SIZE;
		}

		do {
			std::vector<uint8_t> output(output_size);

			Bytef* dst = reinterpret_cast<Bytef*>(&output[0]);
			uLongf dst_len = output.size();
			const Bytef* src = reinterpret_cast<const Bytef*>(&data[0]);

			const int result = uncompress(dst, &dst_len, src, length);
			if(result == Z_OK) {
				output.resize(dst_len);
				return output;
			}

			output_size *= 2;
		} while(output_size < MAX_OUTPUT_SIZE);

		ASSERT_LOG(false, "COULD NOT DECOMPRESS " << length << " BYTE BUFFER\n");

	}

}
