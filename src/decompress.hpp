#pragma once

#include <cstdint>
#include <vector>

namespace zip
{
	std::vector<uint8_t> decompress(const std::vector<uint8_t>& data);
	std::vector<uint8_t> decompress(const uint8_t* data, size_t length);
}
