#pragma once

#include "swf_shape.hpp"

namespace graphics
{
	// virtual base class for generating the list of commands used to make a shape
	class shape
	{
	public:
		shape();
		virtual ~shape();

		void prepare_shape();

		void draw() const;
	private:
	};
}
