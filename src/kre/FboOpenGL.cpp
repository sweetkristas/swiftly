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

#include <GL/glew.h>

#include <cstddef>

#include "asserts.hpp"
#include "DisplayDevice.hpp"
#include "FboOpenGL.hpp"
#include "TextureUtils.hpp"
#include "WindowManager.hpp"

namespace KRE
{
	FboOpenGL::FboOpenGL(unsigned width, unsigned height, 
		unsigned color_plane_count, 
		bool depth, 
		bool stencil, 
		bool use_multi_sampling, 
		unsigned multi_samples)
		: RenderTarget(width, height, color_plane_count, depth, stencil, use_multi_sampling, multi_samples),
		uses_ext_(false),
		depth_stencil_buffer_id_(0),
		tex_width_(0),
		tex_height_(0)
	{
	}

	FboOpenGL::FboOpenGL(const variant& node)
		: RenderTarget(node),
		uses_ext_(false),
		depth_stencil_buffer_id_(0),
		tex_width_(0),
		tex_height_(0)
	{
	}

	FboOpenGL::FboOpenGL(const FboOpenGL& op)
		: RenderTarget(op),
		uses_ext_(false),
		depth_stencil_buffer_id_(0),
		tex_width_(0),
		tex_height_(0)
	{
		if(op.tex_height_ != 0 && op.tex_width_ != 0) {
			create();
		}
	}

	void FboOpenGL::handleCreate()
	{
		GLenum depth_stencil_internal_format;
		GLenum ds_attachment;
		getDSInfo(ds_attachment, depth_stencil_internal_format);

		tex_width_ = next_power_of_two(width());
		tex_height_ = next_power_of_two(height());

		// check for fbo support
		if(GLEW_ARB_framebuffer_object) {
			// XXX we need to add some hints about what size depth and stencil buffers to use.
			if(usesMultiSampling()) {
				ASSERT_LOG(false, "skipped creation of multi-sample render buffers with multiple color planes, for now, as it was hurting my head to think about.");
				/*int render_buffer_count = 1;
				if(DepthPlane() || StencilPlane()) {
					render_buffer_count = 2;
				}
				render_buffer_id_ = std::shared_ptr<std::vector<GLuint>>(new std::vector<GLuint>, [render_buffer_count](std::vector<GLuint>* id){
					glBindRenderbuffer(GL_RENDERBUFFER, 0); 
					glDeleteRenderbuffers(render_buffer_count, &(*id)[0]); 
					delete[] id;
				});
				render_buffer_id_->resize(render_buffer_count);
				glGenRenderbuffers(render_buffer_count, &(*render_buffer_id_)[0]);
				glBindRenderbuffer(GL_RENDERBUFFER, (*render_buffer_id_)[0]);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, MultiSamples(), GL_RGBA8, Width(), Height());
				if(render_buffer_count > 1) {
					glBindRenderbuffer(GL_RENDERBUFFER, (*render_buffer_id_)[1]);
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, MultiSamples(), depth_stencil_internal_format, Width(), Height());
				}
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				// Create Other FBO
				final_texture_id_ = boost::shared_array<GLuint>(new GLuint[2], [](GLuint* id){glDeleteTextures(2,id); delete[] id;});
				glGenTextures(2, &final_texture_id_[0]);
				glBindTexture(GL_TEXTURE_2D, final_texture_id_[0]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width(), Height(), 0, GL_RGBA8, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
				glBindTexture(GL_TEXTURE_2D, final_texture_id_[1] );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
				glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width(), Height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
				glBindTexture(GL_TEXTURE_2D, 0);

				sample_framebuffer_id_ = std::shared_ptr<GLuint>(new GLuint, [](GLuint* id) {
					glDeleteFramebuffers(1, id); 
					delete id;
				});
				glGenFramebuffers(1, sample_framebuffer_id_.get());
				glBindFramebuffer(GL_FRAMEBUFFER, *sample_framebuffer_id_);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, &(*render_buffer_id_)[0]);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id_[1]);
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				ASSERT_LOG(status != GL_FRAMEBUFFER_UNSUPPORTED, "Framebuffer not supported error.");
				ASSERT_LOG(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer completion status not indicated: " << status);

				framebuffer_id_ = std::shared_ptr<GLuint>(new GLuint, [](GLuint* id) {
					glDeleteFramebuffers(1, id); 
					delete id;
				});
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_[0]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, final_texture_id_[0], 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, final_texture_id_[1], 0);
				status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				ASSERT_LOG(status != GL_FRAMEBUFFER_UNSUPPORTED, "Framebuffer not supported error.");
				ASSERT_LOG(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer completion status not indicated: " << status);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

			} else {
				if(getDepthPlane() || getStencilPlane()) {
					depth_stencil_buffer_id_ = std::shared_ptr<GLuint>(new GLuint, [](GLuint* id){ 
						glBindRenderbuffer(GL_RENDERBUFFER, 0); 
						glDeleteRenderbuffers(1, id); 
						delete id; 
					});
					glGenRenderbuffers(1, depth_stencil_buffer_id_.get());
					glBindRenderbuffer(GL_RENDERBUFFER, *depth_stencil_buffer_id_);
					glRenderbufferStorage(GL_RENDERBUFFER, depth_stencil_internal_format, tex_width_, tex_height_);
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
				}

				// Use CreateMaterial.
				auto dd = DisplayDevice::getCurrent();
				std::vector<TexturePtr> textures;
				unsigned color_planes = getColorPlanes();
				textures.reserve(color_planes);
				for(unsigned n = 0; n != color_planes; ++n) {
					textures.emplace_back(dd->createTexture(tex_width_, tex_height_, PixelFormat::PF::PIXELFORMAT_BGRA8888));
				}
				auto mat = dd->createMaterial("fbo_mat", textures);
				mat->setCoords(rect(0, 0, width(), height()));
				setMaterial(mat);

				framebuffer_id_ = std::shared_ptr<GLuint>(new GLuint, [](GLuint* id) {
					glDeleteFramebuffers(1, id); 
					delete id;
				});
				glGenFramebuffers(1, framebuffer_id_.get());
				glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer_id_);
				// attach the texture to FBO color attachment point
				unsigned n = 0;
				for(auto t : textures) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+n, GL_TEXTURE_2D, t->id(), 0);
					++n;
				}
				if(depth_stencil_buffer_id_) {
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, ds_attachment, GL_RENDERBUFFER, *depth_stencil_buffer_id_);
				}
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				ASSERT_LOG(status != GL_FRAMEBUFFER_UNSUPPORTED, "Framebuffer not supported error.");
				ASSERT_LOG(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer completion status not indicated: " << status);
			}
		} else if(GLEW_EXT_framebuffer_object) {
			ASSERT_LOG(!(usesMultiSampling() && !GLEW_EXT_framebuffer_multisample), "Multi-sample texture requested but hardware doesn't support multi-sampling.");
			ASSERT_LOG(!(getDepthPlane() || getStencilPlane() && !GLEW_EXT_packed_depth_stencil), "Depth or Stencil plane required but hardware doesn't support it.");
			uses_ext_ = true;
			// XXX wip
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		setOrder(999999);
	}

	FboOpenGL::~FboOpenGL()
	{
	}

	void FboOpenGL::preRender(const WindowManagerPtr& wnd)
	{
		ASSERT_LOG(framebuffer_id_ != NULL, "Framebuffer object hasn't been created.");
		// XXX wip
		if(sample_framebuffer_id_) {
			// using multi-sampling
			// blit from multisample FBO to final FBO
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_id_[1]);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id_[0]);
			//glBlitFramebuffer(0, 0, Width(), Height(), 0, 0, Width(), Height(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_LINEAR);
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		Blittable::preRender(wnd);
	}

	void FboOpenGL::handleApply()
	{
		ASSERT_LOG(framebuffer_id_ != NULL, "Framebuffer object hasn't been created.");
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer_id_);

		glGetIntegerv(GL_VIEWPORT, viewport_);
		glViewport(0, 0, width(), height());
	}

	void FboOpenGL::handleUnapply()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
	}

	void FboOpenGL::handleClear()
	{
		ASSERT_LOG(framebuffer_id_ != NULL, "Framebuffer object hasn't been created.");
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer_id_);
		auto& color = getClearColor();
		glClearColor(color.rf(), color.gf(), color.bf(), color.af());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	RenderTargetPtr FboOpenGL::handleClone()
	{
		return std::make_shared<FboOpenGL>(*this);
	}

	void FboOpenGL::getDSInfo(GLenum& ds_attachment, GLenum& depth_stencil_internal_format)
	{
		if(getDepthPlane() || getStencilPlane()) {
			if(getDepthPlane()) {
				if(getStencilPlane()) {
					depth_stencil_internal_format = GL_DEPTH24_STENCIL8;
					ds_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
				} else {
					depth_stencil_internal_format = GL_DEPTH_COMPONENT16;
					ds_attachment = GL_DEPTH_ATTACHMENT;
				}
			} else {
				depth_stencil_internal_format = GL_STENCIL_INDEX8;
				ds_attachment = GL_STENCIL_ATTACHMENT;
			}
		}
	}
}
