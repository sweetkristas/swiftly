#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace geometry
{
	struct point2d_twip
	{
		point2d_twip() : x(0), y(0) {}
		explicit point2d_twip(int32_t xx, int32_t yy) : x(xx), y(yy) {}
		int32_t x;
		int32_t y;
	};

	struct rect
	{
		size_t x1;
		size_t y1;
		size_t x2;
		size_t y2;
	};

	void generate_triangles(const std::vector<point2d_twip>& points, const rect& bounds, std::vector<float>& triangles);
}
