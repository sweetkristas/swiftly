#pragma once

#include "bit_reader.hpp"
#include "swf_fwd.hpp"

namespace swf
{
	class code_block
	{
	public:
		explicit code_block(const codestream& codes, codestream_iterator start, codestream_iterator end)
		{
			std::copy(start, end, std::back_inserter(codestream_));
		}

		explicit code_block(const codestream& codes) 
			: codestream_(codes)
		{
		}

		codestream_iterator begin() { return codestream_.begin(); }
		codestream_iterator end() { return codestream_.end(); }

		code_block(codestream_iterator start, codestream_iterator end)
		{
			std::copy(start, end, std::back_inserter(codestream_));
		}

		static code_block convert_bitstream_to_codestream(const bit_stream_ptr& bits) {
			bool finished = false;
			codestream cs;
			while(!finished) {
				uint8_t code = bits->read_unsigned8();
				if(code == 0) {
					finished = true;
				}
				cs.emplace_back(code);
				if(code >= 128) {
					uint16_t len = bits->read_unsigned16();
					cs.emplace_back(len & 0xff);
					cs.emplace_back(len >> 8);
					auto ary = bits->read_unsigned8(len);
					cs.insert(cs.end(), ary.begin(), ary.end());
				}
			}
			return code_block(cs);
		}
	private:
		codestream codestream_;
	};
}
