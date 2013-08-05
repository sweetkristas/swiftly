#pragma once

#include "swf_shape.hpp"

namespace graphics
{
	class shape
	{
		shape();
		shape(const swf::shape& shape);
		virtual ~shape();

		void draw() const;
		void prepare(const swf::shape& shape);	
	private:
		bool ready_for_draw_;

		shape(const shape&);
	};
}
