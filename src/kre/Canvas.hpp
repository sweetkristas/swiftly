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

#pragma once

#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Color.hpp"
#include "geometry.hpp"
#include "Material.hpp"
#include "Util.hpp"
#include "VGraph.hpp"
#include "SceneUtil.hpp"

namespace KRE
{
	class Canvas;
	typedef std::shared_ptr<Canvas> CanvasPtr;

	// A 2D canvas class for drawing on. Not in the renderable pipelines.
	// Canvas writes are done in the order in the code.
	// Intended for making things like UI's.
	class Canvas
	{
	public:
		virtual ~Canvas();

		void setDimensions(unsigned w, unsigned h);

		unsigned width() const { return width_; }
		unsigned height() const { return height_; }

		// Blit's a texture from co-ordinates given in src to the screen co-ordinates dst
		virtual void blitTexture(const TexturePtr& tex, const rect& src, float rotation, const rect& dst, const Color& color=Color::colorWhite()) const = 0;
		virtual void blitTexture(const TexturePtr& tex, const std::vector<vertex_texcoord>& vtc, float rotation, const Color& color=Color::colorWhite()) = 0;
		// Blit a texture to the given co-ordinates on the display. Assumes the whole texture is being used.
		void blitTexture(const TexturePtr& tex, float rotation, const rect& dst, const Color& color=Color::colorWhite()) const;
		void blitTexture(const TexturePtr& tex, float rotation, int x, int y, const Color& color=Color::colorWhite()) const;

		// Blit's a material from internal co-ordinates to destination screen co-ordinates.
		virtual void blitTexture(const MaterialPtr& mat, float rotation, const rect& dst, const Color& color=Color::colorWhite()) const = 0;
		virtual void blitTexture(const MaterialPtr& mat, const rect& src, float rotation, const rect& dst, const Color& color=Color::colorWhite()) const = 0;

		virtual void drawSolidRect(const rect& r, const Color& fill_color, const Color& stroke_color, float rotate=0) const = 0;
		virtual void drawSolidRect(const rect& r, const Color& fill_color, float rotate=0) const = 0;
		virtual void drawHollowRect(const rect& r, const Color& stroke_color, float rotate=0) const = 0;
		virtual void drawLine(const point& p1, const point& p2, const Color& color) const = 0;
		virtual void drawLines(const std::vector<glm::vec2>& varray, float line_width, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawLines(const std::vector<glm::vec2>& varray, float line_width, const std::vector<glm::u8vec4>& carray) const = 0;
		virtual void drawLineStrip(const std::vector<glm::vec2>& points, float line_width, const Color& color) const = 0;
		virtual void drawLineLoop(const std::vector<glm::vec2>& varray, float line_width, const Color& color) const = 0;
		virtual void drawLine(const pointf& p1, const pointf& p2, const Color& color) const = 0;
		// Draw filled polygon (i.e. triangle fan) using given color	
		// Should add a version taking fill and stroke color.
		virtual void drawPolygon(const std::vector<glm::vec2>& points, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawSolidCircle(const point& centre, double radius, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawSolidCircle(const point& centre, double radius, const std::vector<uint8_t>& color) const = 0;
		virtual void drawHollowCircle(const point& centre, double radius, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawSolidCircle(const pointf& centre, double radius, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawSolidCircle(const pointf& centre, double radius, const std::vector<uint8_t>& color) const = 0;
		virtual void drawHollowCircle(const pointf& centre, double radius, const Color& color=Color::colorWhite()) const = 0;
		virtual void drawPoints(const std::vector<glm::vec2>& points, float radius, const Color& color=Color::colorWhite()) const = 0;

		void drawVectorContext(const Vector::ContextPtr& context);

		static CanvasPtr getInstance();

		struct ColorManager
		{
			ColorManager(const Color& color) : canvas_(KRE::Canvas::getInstance()) {
				canvas_->color_stack_.push(color);
			}
			~ColorManager() {
				canvas_->color_stack_.pop();
			}
			CanvasPtr canvas_;
		};

		struct ModelManager
		{
			ModelManager(int tx, int ty, float rotation, float scale) : canvas_(KRE::Canvas::getInstance()) {
				glm::mat4 m_trans	= glm::translate(glm::mat4(1.0f), glm::vec3(static_cast<float>(tx), static_cast<float>(ty),0.0f));
				glm::mat4 m_rotate  = glm::rotate(m_trans, rotation, glm::vec3(0.0f,0.0f,1.0f));
				glm::mat4 model		= glm::scale(m_rotate, glm::vec3(scale));
				if(!canvas_->model_stack_.empty()) {
					model = canvas_->model_stack_.top() * model;
				}
				canvas_->model_stack_.emplace(model);
			}
			~ModelManager() {
				canvas_->model_stack_.pop();
			}
			CanvasPtr canvas_;
		};

		const glm::mat4& getModelMatrix() const 
		{
			ASSERT_LOG(!model_stack_.empty(), "Model stack was empty");
			return model_stack_.top();
		}

		const Color getColor() const {
			if(color_stack_.empty()) {
				return Color::colorWhite();
			}
			return color_stack_.top();
		}

	protected:
		Canvas();
	private:
		DISALLOW_COPY_AND_ASSIGN(Canvas);
		unsigned width_;
		unsigned height_;
		virtual void handleDimensionsChanged() = 0;
		std::stack<Color> color_stack_;
		std::stack<glm::mat4> model_stack_;
	};

	Color operator*(const Color& lhs, const Color& rhs);
}
