#pragma once

#include <vector>
#include <set>

namespace geometry
{
	class point2d
	{
	public:
		point2d() : x_(0), y_(0)
		{}
		explicit point2d(int x, int y) : x_(x), y_(y)
		{}
		virtual ~point2d()
		{}
		int x() const { return x_; }
		int y() const { return y_; }
		bool operator<(const point2d& rhs) {
			if(y_ < rhs.y_ || y_ == rhs.y_ && x_ <= rhs.x_) {
				return true;
			}
			return false;
		}
	private:
		int x_;
		int y_;
	};

	typedef std::vector<point2d> point_array;
	typedef std::pair<point2d, point2d> edge;

	class polygon
	{
	public:
		polygon();
		explicit polygon(point_array points);
		virtual ~polygon();
		
		void monotonate();
	private:
		std::vector<edge> edges_;
		std::set<point2d> vertices_;
	};
}
