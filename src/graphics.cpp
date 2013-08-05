#include "asserts.hpp"
#include "graphics.hpp"

namespace graphics
{
	shape::shape() : ready_for_draw_(false)
	{
	}

	shape::shape(const swf::shape& shape) : ready_for_draw_(false)
	{
		prepare(shape);
	}

	shape::~shape()
	{
	}

	void shape::prepare(const swf::shape& shape)
	{
		for(auto sr : shape.get_shape_records()) {
			
		}


		ready_for_draw_ = true;
	}

	void shape::draw() const
	{
		ASSERT_LOG(ready_for_draw_, "Drawing starting but shape wasn't prepared for drawing.");

		// XXX
	}
}
