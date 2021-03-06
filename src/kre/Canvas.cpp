/*
	Copyright (C) 2013-2014 by Kristina Simpson <sweet.kristas@gmail.com>
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgement in the product documentation would be
	   appreciated but is not required.

	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	   3. This notice may not be removed or altered from any source
	   distribution.
*/

#include "Canvas.hpp"
#include "DisplayDevice.hpp"

namespace KRE
{
	Canvas::Canvas()
		: width_(0),
		height_(0)
	{
		model_stack_.emplace(glm::mat4(1.0f));
	}

	void Canvas::setDimensions(unsigned w, unsigned h)
	{
		width_ = w;
		height_ = h;
		handleDimensionsChanged();
	}

	Canvas::~Canvas()
	{
	}

	CanvasPtr Canvas::getInstance()
	{
		return DisplayDevice::getCurrent()->getCanvas();
	}

	void Canvas::drawVectorContext(const Vector::ContextPtr& context)
	{
		// XXX This almost feels like a little hack.
		// XXX Probably want to call a context->Draw() function or something similar.
		// XXX maybe. It's kind of tricky and I feel the abstraction isn't quite right yet.
		// Since we may have a more efficient path using opengl to draw stuff, rather than blitting a cairo texture.
		// Maybe renderable should have an overridable draw function that can be called?
		// then in DisplayDevice::Render() we check it and run it.
		//DisplayDevice::getCurrent()->render(context);
		ASSERT_LOG(false, "drawVectorContext fixme");
	}

	void Canvas::blitTexture(const TexturePtr& tex, float rotation, const rect& dst, const Color& color) const
	{
		blitTexture(tex, rect(0,0,0,0), rotation, dst, color);
	}

	void Canvas::blitTexture(const TexturePtr& tex, float rotation, int x, int y, const Color& color) const
	{
		blitTexture(tex, rect(0,0,0,0), rotation, rect(x,y), color);
	}
}
