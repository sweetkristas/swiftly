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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "asserts.hpp"
#include "ShadersOpenGL.hpp"

namespace KRE
{
	namespace OpenGL
	{
		namespace
		{
			struct uniform_mapping { const char* alt_name; const char* name; };
			struct attribute_mapping { const char* alt_name; const char* name; };

			const char* const default_vs = 
				"uniform mat4 u_mvp_matrix;\n"
				"attribute vec2 a_position;\n"
				"attribute vec2 a_texcoord;\n"
				"varying vec2 v_texcoord;\n"
				"void main()\n"
				"{\n"
				"    v_texcoord = a_texcoord;\n"
				"    gl_Position = u_mvp_matrix * vec4(a_position,0.0,1.0);\n"
				"}\n";
			const char* const default_fs =
				"uniform sampler2D u_tex_map;\n"
				"varying vec2 v_texcoord;\n"
				"uniform bool u_discard;\n"
				"uniform vec4 u_color;\n"
				"void main()\n"
				"{\n"
				"    vec4 color = texture2D(u_tex_map, v_texcoord);\n"
				"    if(u_discard && color[3] == 0.0) {\n"
				"        discard;\n"
				"    } else {\n"
				"        gl_FragColor = color * u_color;\n"
				"    }\n"
				"}\n";

			const uniform_mapping default_uniform_mapping[] =
			{
				{"mvp_matrix", "u_mvp_matrix"},
				{"color", "u_color"},
				{"discard", "u_discard"},
				{"tex_map", "u_tex_map"},
				{"tex_map0", "u_tex_map"},
				{"", ""},
			};
			const attribute_mapping default_attribue_mapping[] =
			{
				{"position", "a_position"},
				{"texcoord", "a_texcoord"},
				{"", ""},
			};

			const char* const simple_vs = 
				"uniform mat4 u_mvp_matrix;\n"
				"uniform float u_point_size;\n"
				"attribute vec2 a_position;\n"
				"void main()\n"
				"{\n"
				"    gl_PointSize = u_point_size;\n"
				"    gl_Position = u_mvp_matrix * vec4(a_position,0.0,1.0);\n"
				"}\n";
			const char* const simple_fs =
				"uniform bool u_discard;\n"
				"uniform vec4 u_color;\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = u_color;\n"
				"    if(u_discard && gl_FragColor[3] == 0.0) {\n"
				"        discard;\n"
				"    }\n"
				"}\n";

			const uniform_mapping simple_uniform_mapping[] =
			{
				{"mvp_matrix", "u_mvp_matrix"},
				{"color", "u_color"},
				{"discard", "u_discard"},
				{"point_size", "u_point_size"},
				{"", ""},
			};
			const attribute_mapping simple_attribue_mapping[] =
			{
				{"position", "a_position"},
				{"", ""},
			};

			const char* const attr_color_vs = 
				"uniform mat4 u_mvp_matrix;\n"
				"uniform float u_point_size;\n"
				"attribute vec2 a_position;\n"
				"attribute vec4 a_color;\n"
				"varying vec4 v_color;\n"
				"void main()\n"
				"{\n"
				"	 v_color = a_color;\n"
				"    gl_PointSize = u_point_size;\n"
				"    gl_Position = u_mvp_matrix * vec4(a_position,0.0,1.0);\n"
				"}\n";
			const char* const attr_color_fs =
				"uniform bool u_discard;\n"
				"uniform vec4 u_color;\n"
				"varying vec4 v_color;\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = v_color * u_color;\n"
				"    if(u_discard && gl_FragColor[3] == 0.0) {\n"
				"        discard;\n"
				"    }\n"
				"}\n";

			const uniform_mapping attr_color_uniform_mapping[] =
			{
				{"mvp_matrix", "u_mvp_matrix"},
				{"color", "u_color"},
				{"discard", "u_discard"},
				{"point_size", "u_point_size"},
				{"", ""},
			};
			const attribute_mapping attr_color_attribue_mapping[] =
			{
				{"position", "a_position"},
				{"color", "a_color"},
				{"", ""},
			};

			const char* const vtc_vs = 
				"uniform mat4 u_mvp_matrix;\n"
				"attribute vec2 a_position;\n"
				"attribute vec2 a_texcoord;\n"
				"attribute vec4 a_color;\n"
				"varying vec2 v_texcoord;\n"
				"varying vec4 v_color;\n"
				"void main()\n"
				"{\n"
				"    v_color = a_color;\n"
				"    v_texcoord = a_texcoord;\n"
				"    gl_Position = u_mvp_matrix * vec4(a_position,0.0,1.0);\n"
				"}\n";
			const char* const vtc_fs =
				"uniform sampler2D u_tex_map;\n"
				"varying vec2 v_texcoord;\n"
				"varying vec4 v_color;\n"
				"uniform vec4 u_color;\n"
				"void main()\n"
				"{\n"
				"    vec4 color = texture2D(u_tex_map, v_texcoord);\n"
				"    gl_FragColor = color * v_color * u_color;\n"
				"}\n";

			const uniform_mapping vtc_uniform_mapping[] =
			{
				{"mvp_matrix", "u_mvp_matrix"},
				{"color", "u_color"},
				{"tex_map", "u_tex_map"},
				{"tex_map0", "u_tex_map"},
				{"", ""},
			};
			const attribute_mapping vtc_attribue_mapping[] =
			{
				{"position", "a_position"},
				{"texcoord", "a_texcoord"},
				{"color", "a_color"},
				{"", ""},
			};

			const struct {
				const char* shader_name;
				const char* vertex_shader_name;
				const char* const vertex_shader_data;
				const char* fragment_shader_name;
				const char* const fragment_shader_data;
				const uniform_mapping* u_mapping;
				const attribute_mapping* a_mapping;
			} shader_defs[] = 
			{
				{ "default", "default_vs", default_vs, "default_fs", default_fs, default_uniform_mapping, default_attribue_mapping },
				{ "simple", "simple_vs", simple_vs, "simple_fs", simple_fs, simple_uniform_mapping, simple_attribue_mapping },
				{ "attr_color_shader", "attr_color_vs", attr_color_vs, "attr_color_fs", attr_color_fs, attr_color_uniform_mapping, attr_color_attribue_mapping },
				{ "vtc_shader", "vtc_vs", vtc_vs, "vtc_fs", vtc_fs, vtc_uniform_mapping, vtc_attribue_mapping },
			};

			typedef std::map<std::string, ShaderProgramPtr> shader_factory_map;
			shader_factory_map& get_shader_factory()
			{
				static shader_factory_map res;
				if(res.empty()) {
					// XXX load some default shaders here.
					for(auto& def : shader_defs) {
						auto spp = new OpenGL::ShaderProgram(def.shader_name, 
							ShaderDef(def.vertex_shader_name, def.vertex_shader_data),
							ShaderDef(def.fragment_shader_name, def.fragment_shader_data),
							variant());
						res[def.shader_name] = ShaderProgramPtr(spp);
						auto um = def.u_mapping;
						while(strlen(um->alt_name) > 0) {
							spp->setAlternateUniformName(um->name, um->alt_name);
							++um;
						}
						auto am = def.a_mapping;
						while(strlen(am->alt_name) > 0) {
							spp->setAlternateAttributeName(am->name, am->alt_name);
							++am;
						}
						spp->setActives();
					}
				}
				return res;
			}
		}

		Shader::Shader(GLenum type, const std::string& name, const std::string& code)
			: type_(type), shader_(0), name_(name)
		{
			bool compiled_ok = compile(code);
			ASSERT_LOG(compiled_ok == true, "Error compiling shader for " << name_);
		}

		bool Shader::compile(const std::string& code)
		{
			GLint compiled;
			if(shader_) {
				glDeleteShader(shader_);
				shader_ = 0;
			}

			ASSERT_LOG(glCreateShader != NULL, "Something bad happened with Glew shader not initialised.");
			shader_ = glCreateShader(type_);
			if(shader_ == 0) {
				std::cerr << "Enable to create shader." << std::endl;
				return false;
			}
			const char* shader_code = code.c_str();
			glShaderSource(shader_, 1, &shader_code, NULL);
			glCompileShader(shader_);
			glGetShaderiv(shader_, GL_COMPILE_STATUS, &compiled);
			if(!compiled) {
				GLint info_len = 0;
				glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &info_len);
				if(info_len > 1) {
					std::vector<char> info_log;
					info_log.resize(info_len);
					glGetShaderInfoLog(shader_, info_log.capacity(), NULL, &info_log[0]);
					std::string s(info_log.begin(), info_log.end());
					std::cerr << "Error compiling shader: " << s << std::endl;
				}
				glDeleteShader(shader_);
				shader_ = 0;
				return false;
			}
			return true;
		}

		ShaderProgram::ShaderProgram(const std::string& name, const ShaderDef& vs, const ShaderDef& fs, const variant& node)
			: KRE::ShaderProgram(node),
			object_(0)
		{
			init(name, vs, fs);
		}

		ShaderProgram::~ShaderProgram()
		{
			if(object_ != 0) {
				glDeleteShader(object_);
				object_ = 0;
			}
		}

		void ShaderProgram::init(const std::string& name, const ShaderDef& vs, const ShaderDef& fs)
		{
			name_ = name;
			vs_.reset(new Shader(GL_VERTEX_SHADER, vs.first, vs.second));
			fs_.reset(new Shader(GL_FRAGMENT_SHADER, fs.first, fs.second));
			bool linked_ok = link();
			ASSERT_LOG(linked_ok == true, "Error linking program: " << name_);
		}

		GLint ShaderProgram::getAttributeOrDie(const std::string& attr) const
		{
			return getAttributeIterator(attr)->second.location;
		}

		GLint ShaderProgram::getUniformOrDie(const std::string& attr) const
		{
			return getUniformIterator(attr)->second.location;
		}

		GLint ShaderProgram::getAttribute(const std::string& attr) const
		{
			auto it = attribs_.find(attr);
			if(it != attribs_.end()) {
				return it->second.location;
			}
			auto alt_name_it = attribute_alternate_name_map_.find(attr);
			if(alt_name_it == attribute_alternate_name_map_.end()) {
				LOG_WARN("Attribute '" << attr << "' not found in alternate names list and is not a name defined in the shader: " << name_);
				return GLint(-1);
			}
			it = attribs_.find(alt_name_it->second);
			if(it == attribs_.end()) {
				LOG_WARN("Attribute \"" << alt_name_it->second << "\" not found in list, looked up from symbol " << attr << " in shader: " << name_);
				return GLint(-1);
			}
			return it->second.location;
		}

		GLint ShaderProgram::getUniform(const std::string& attr) const
		{
			auto it = uniforms_.find(attr);
			if(it != uniforms_.end()) {
				return it->second.location;
			}
			auto alt_name_it = uniform_alternate_name_map_.find(attr);
			if(alt_name_it == uniform_alternate_name_map_.end()) {
				LOG_WARN("Uniform '" << attr << "' not found in alternate names list and is not a name defined in the shader: " << name_);
				return GLint(-1);
			}
			it = uniforms_.find(alt_name_it->second);
			if(it == uniforms_.end()) {
				LOG_WARN("Uniform \"" << alt_name_it->second << "\" not found in list, looked up from symbol " << attr << " in shader: " << name_);
				return GLint(-1);
			}
			return it->second.location;
		}

		ConstActivesMapIterator ShaderProgram::getAttributeIterator(const std::string& attr) const
		{
			auto it = attribs_.find(attr);
			if(it == attribs_.end()) {
				auto alt_name_it = attribute_alternate_name_map_.find(attr);
				ASSERT_LOG(alt_name_it != attribute_alternate_name_map_.end(), 
					"Attribute '" << attr << "' not found in alternate names list and is not a name defined in the shader: " << name_);
				it = attribs_.find(alt_name_it->second);
				ASSERT_LOG(it != attribs_.end(), 
					"Attribute \"" << alt_name_it->second << "\" not found in list, looked up from symbol " << attr << " in shader: " << name_);
			}
			return it;
		}

		ConstActivesMapIterator ShaderProgram::getUniformIterator(const std::string& attr) const
		{
			auto it = uniforms_.find(attr);
			if(it == uniforms_.end()) {
				auto alt_name_it = uniform_alternate_name_map_.find(attr);
				ASSERT_LOG(alt_name_it != uniform_alternate_name_map_.end(), 
					"Uniform '" << attr << "' not found in alternate names list and is not a name defined in the shader: " << name_);
				it = uniforms_.find(alt_name_it->second);
				ASSERT_LOG(it != uniforms_.end(), 
					"Uniform \"" << alt_name_it->second << "\" not found in list, looked up from symbol " << attr << " in shader: " << name_);
			}
			return it;
		}

		bool ShaderProgram::link()
		{
			if(object_) {
				glDeleteProgram(object_);
				object_ = 0;
			}
			object_ = glCreateProgram();
			ASSERT_LOG(object_ != 0, "Unable to create program object.");
			glAttachShader(object_, vs_->get());
			glAttachShader(object_, fs_->get());
			glLinkProgram(object_);
			GLint linked = 0;
			glGetProgramiv(object_, GL_LINK_STATUS, &linked);
			if(!linked) {
				GLint info_len = 0;
				glGetProgramiv(object_, GL_INFO_LOG_LENGTH, &info_len);
				if(info_len > 1) {
					std::vector<char> info_log;
					info_log.resize(info_len);
					glGetProgramInfoLog(object_, info_log.capacity(), NULL, &info_log[0]);
					std::string s(info_log.begin(), info_log.end());
					std::cerr << "Error linking object: " << s << std::endl;
				}
				glDeleteProgram(object_);
				object_ = 0;
				return false;
			}
			return queryUniforms() && queryAttributes();
		}

		bool ShaderProgram::queryUniforms()
		{
			GLint active_uniforms;
			glGetProgramiv(object_, GL_ACTIVE_UNIFORMS, &active_uniforms);
			GLint uniform_max_len;
			glGetProgramiv(object_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_max_len);
			std::vector<char> name;
			name.resize(uniform_max_len+1);
			for(int i = 0; i < active_uniforms; i++) {
				Actives u;
				GLsizei size;
				glGetActiveUniform(object_, i, name.size(), &size, &u.num_elements, &u.type, &name[0]);
				u.name = std::string(&name[0], &name[size]);
				u.location = glGetUniformLocation(object_, u.name.c_str());
				ASSERT_LOG(u.location >= 0, "Unable to determine the location of the uniform: " << u.name);
				uniforms_[u.name] = u;
			}
			return true;
		}

		bool ShaderProgram::queryAttributes()
		{
			GLint active_attribs;
			glGetProgramiv(object_, GL_ACTIVE_ATTRIBUTES, &active_attribs);
			GLint attributes_max_len;
			glGetProgramiv(object_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributes_max_len);
			std::vector<char> name;
			name.resize(attributes_max_len+1);
			for(int i = 0; i < active_attribs; i++) {
				Actives a;
				GLsizei size;
				glGetActiveAttrib(object_, i, name.size(), &size, &a.num_elements, &a.type, &name[0]);
				a.name = std::string(&name[0], &name[size]);
				a.location = glGetAttribLocation(object_, a.name.c_str());
				ASSERT_LOG(a.location >= 0, "Unable to determine the location of the attribute: " << a.name);
				ASSERT_LOG(a.num_elements == 1, "More than one element was found for an attribute(" << a.name << ") in shader(" << this->name() << "): " << a.num_elements);
				attribs_[a.name] = a;
			}
			return true;
		}

		void ShaderProgram::makeActive()
		{
			glUseProgram(object_);
		}

		void ShaderProgram::setUniformValue(ConstActivesMapIterator it, const void* value)
		{
			const Actives& u = it->second;
			ASSERT_LOG(value != NULL, "setUniformValue(): value is NULL");
			switch(u.type) {
			case GL_INT:
			case GL_BOOL:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:	
				glUniform1i(u.location, *(GLint*)value); 
				break;
			case GL_INT_VEC2:	
			case GL_BOOL_VEC2:	
				glUniform2i(u.location, ((GLint*)value)[0], ((GLint*)value)[1]); 
				break;
			case GL_INT_VEC3:	
			case GL_BOOL_VEC3:	
				glUniform3iv(u.location, u.num_elements, (GLint*)value); 
				break;
			case GL_INT_VEC4: 	
			case GL_BOOL_VEC4:
				glUniform4iv(u.location, u.num_elements, (GLint*)value); 
				break;

			case GL_FLOAT: {
				glUniform1f(u.location, *(GLfloat*)value);
				break;
			}
			case GL_FLOAT_VEC2: {
				glUniform2fv(u.location, u.num_elements, (GLfloat*)value);
				break;
			}
			case GL_FLOAT_VEC3: {
				glUniform3fv(u.location, u.num_elements, (GLfloat*)value);
				break;
			}
			case GL_FLOAT_VEC4: {
				glUniform4fv(u.location, u.num_elements, (GLfloat*)value);
				break;
			}
			case GL_FLOAT_MAT2:	{
				glUniformMatrix2fv(u.location, u.num_elements, GL_FALSE, (GLfloat*)value);
				break;
			}
			case GL_FLOAT_MAT3: {
				glUniformMatrix3fv(u.location, u.num_elements, GL_FALSE, (GLfloat*)value);
				break;
			}
			case GL_FLOAT_MAT4: {
				glUniformMatrix4fv(u.location, u.num_elements, GL_FALSE, (GLfloat*)value);
				break;
			}
			default:
				ASSERT_LOG(false, "Unhandled uniform type: " << it->second.type);
			}
		}

		void ShaderProgram::setUniformValue(ConstActivesMapIterator it, const GLint value)
		{
			const Actives& u = it->second;
			switch(u.type) {
			case GL_INT:
			case GL_BOOL:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:	
				glUniform1i(u.location, value); 
				break;
			default:
				ASSERT_LOG(false, "Unhandled uniform type: " << it->second.type);
			}
		}

		void ShaderProgram::setUniformValue(ConstActivesMapIterator it, const GLfloat value)
		{
			const Actives& u = it->second;
			switch(u.type) {
			case GL_FLOAT: {
				glUniform1f(u.location, value);
				break;
			}
			default:
				ASSERT_LOG(false, "Unhandled uniform type: " << it->second.type);
			}	
		}

		void ShaderProgram::setUniformValue(ConstActivesMapIterator it, const GLint* value)
		{
			const Actives& u = it->second;
			ASSERT_LOG(value != NULL, "set_uniform(): value is NULL");
			switch(u.type) {
			case GL_INT:
			case GL_BOOL:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:	
				glUniform1i(u.location, *value); 
				break;
			case GL_INT_VEC2:	
			case GL_BOOL_VEC2:	
				glUniform2i(u.location, value[0], value[1]); 
				break;
			case GL_INT_VEC3:	
			case GL_BOOL_VEC3:	
				glUniform3iv(u.location, u.num_elements, value); 
				break;
			case GL_INT_VEC4: 	
			case GL_BOOL_VEC4:
				glUniform4iv(u.location, u.num_elements, value); 
				break;
			default:
				ASSERT_LOG(false, "Unhandled uniform type: " << it->second.type);
			}
		}

		void ShaderProgram::setUniformValue(ConstActivesMapIterator it, const GLfloat* value)
		{
			const Actives& u = it->second;
			ASSERT_LOG(value != NULL, "setUniformValue(): value is NULL");
			switch(u.type) {
			case GL_FLOAT: {
				glUniform1f(u.location, *value);
				break;
			}
			case GL_FLOAT_VEC2: {
				glUniform2fv(u.location, u.num_elements, value);
				break;
			}
			case GL_FLOAT_VEC3: {
				glUniform3fv(u.location, u.num_elements, value);
				break;
			}
			case GL_FLOAT_VEC4: {
				glUniform4fv(u.location, u.num_elements, value);
				break;
			}
			case GL_FLOAT_MAT2:	{
				glUniformMatrix2fv(u.location, u.num_elements, GL_FALSE, value);
				break;
			}
			case GL_FLOAT_MAT3: {
				glUniformMatrix3fv(u.location, u.num_elements, GL_FALSE, value);
				break;
			}
			case GL_FLOAT_MAT4: {
				glUniformMatrix4fv(u.location, u.num_elements, GL_FALSE, value);
				break;
			}
			default:
				ASSERT_LOG(false, "Unhandled uniform type: " << it->second.type);
			}	
		}

		void ShaderProgram::setAlternateUniformName(const std::string& name, const std::string& alt_name)
		{
			ASSERT_LOG(uniform_alternate_name_map_.find(alt_name) == uniform_alternate_name_map_.end(),
				"Trying to replace alternative uniform name: " << alt_name << " " << name);
			uniform_alternate_name_map_[alt_name] = name;
		}

		void ShaderProgram::setAlternateAttributeName(const std::string& name, const std::string& alt_name)
		{
			ASSERT_LOG(attribute_alternate_name_map_.find(alt_name) == attribute_alternate_name_map_.end(),
				"Trying to replace alternative attribute name: " << alt_name << " " << name);
			attribute_alternate_name_map_[alt_name] = name;
		}

		void ShaderProgram::setActives()
		{
			glUseProgram(object_);
			// Cache some frequently used uniforms.
			if(getUniform("mvp_matrix") != -1) {
				u_mvp_ = getUniformIterator("mvp_matrix");
			} else {
				u_mvp_ = uniforms_.end();
			}
			if(getUniform("color") != -1) {
				u_color_ = getUniformIterator("color");
				setUniformValue(u_color_, glm::value_ptr(glm::vec4(1.0f)));
			} else {
				u_color_ = uniforms_.end();
			}
			if(getUniform("tex_map") != -1) {
				u_tex_ = getUniformIterator("tex_map");
			} else {
				u_tex_ = uniforms_.end();
			}
			if(getAttribute("position") != -1) {
				a_vertex_ = getAttributeIterator("position");
			} else {
				a_vertex_ = attribs_.end();
			}
			if(getAttribute("texcoord") != -1) {
				a_texcoord_ = getAttributeIterator("texcoord");
			} else {
				a_texcoord_ = attribs_.end();
			}
			if(getAttribute("a_color") != -1) {
				a_color_ = getAttributeIterator("a_color");
			} else {
				a_color_ = attribs_.end();
			}
		}

		ActivesHandleBasePtr ShaderProgram::getHandle(const std::string& aname) 
		{
			auto it = uniforms_.find(aname);
			if(it != uniforms_.end()) {
				return std::make_shared<ActivesHandleBase>(ActivesHandle(it));
			}
			auto alt_name_it = uniform_alternate_name_map_.find(aname);
			if(alt_name_it != uniform_alternate_name_map_.end()) {
				it = uniforms_.find(alt_name_it->second);
				if(it != uniforms_.end()) {
					return std::make_shared<ActivesHandleBase>(ActivesHandle(it));
				}
			}

			auto ait = attribs_.find(aname);
			if(ait != attribs_.end()) {
				return std::make_shared<ActivesHandleBase>(ActivesHandle(ait));
			}
			auto aalt_name_it = attribute_alternate_name_map_.find(aname);
			if(aalt_name_it != attribute_alternate_name_map_.end()) {
				ait = attribs_.find(aalt_name_it->second);
				if(ait != attribs_.end()) {
					return std::make_shared<ActivesHandleBase>(ActivesHandle(ait));
				}
			}

			ASSERT_LOG(false, "Couldn't find named active in shader(" << name() << "): " << aname);
			return ActivesHandleBasePtr();
		}

		void ShaderProgram::setUniform(ActivesHandleBasePtr active, const void* value) 
		{
			auto ptr = std::dynamic_pointer_cast<ActivesHandle>(active);
			ASSERT_LOG(ptr != NULL, "Unable to convert active to correct type.");
			setUniformValue(ptr->getIterator(), value);
		}

		/*void ShaderProgram::setAttribute(ActivesHandleBasePtr active, const void* value) 
		{
			auto ptr = std::dynamic_pointer_cast<ActivesHandle>(active);
			ASSERT_LOG(ptr != NULL, "Unable to convert active to correct type.");
			setAttributeValue(ptr->getIterator(), value);
		}*/

		void ShaderProgram::applyActives() 
		{
		}

		ShaderProgramPtr ShaderProgram::factory(const std::string& name)
		{
			auto& sf = get_shader_factory();
			auto it = sf.find(name);
			ASSERT_LOG(it != sf.end(), "Shader '" << name << "' not found in the list of shaders.");
			return it->second;
		}

		ShaderProgramPtr ShaderProgram::defaultSystemShader()
		{
			auto& sf = get_shader_factory();
			auto it = sf.find("default");
			ASSERT_LOG(it != sf.end(), "No 'default' shader found in the list of shaders.");
			return it->second;
		}

		void ShaderProgram::loadFromFile(const variant& node)
		{
			auto& sf = get_shader_factory();

			ASSERT_LOG(node.has_key("instances"), "Shader data must have 'instances' attribute.");
			ASSERT_LOG(node["instances"].is_list(), "'instances' attribute should be a list.");

			for(auto instance : node["instances"].as_list()) {
				ASSERT_LOG(instance.is_map(), "Each item in the 'instances' list must be a map.");
				ASSERT_LOG(instance.has_key("fragment") && instance.has_key("vertex") && instance.has_key("name"), 
					"instances must have 'fragment', 'vertex' and 'name' attributes.");

				const std::string& name = instance["name"].as_string();
				const std::string& vert_data = instance["vertex"].as_string();
				const std::string& frag_data = instance["fragment"].as_string();

				auto spp = new ShaderProgram(name, 
					ShaderDef(name + "_vs", vert_data),
					ShaderDef(name + "_fs", frag_data),
					instance);
				auto it = sf.find(name);
				if(it != sf.end()) {
					LOG_WARN("Overwriting shader with name: " << name);
				}
				sf[name] = ShaderProgramPtr(spp);
				if(instance.has_key("uniforms")) {
					ASSERT_LOG(instance["uniforms"].is_map(), "'uniforms' attribute in shader(" << name << ") must be a map.");
					for(auto uni : instance["uniforms"].as_map()) {
						spp->setAlternateUniformName(uni.first.as_string(), uni.second.as_string());
					}
				}
				if(instance.has_key("attributes")) {
					ASSERT_LOG(instance["attributes"].is_map(), "'attributes' attribute in shader(" << name << ") must be a map.");
					for(auto attr : instance["attributes"].as_map()) {
						spp->setAlternateAttributeName(attr.first.as_string(), attr.second.as_string());
					}
				}
				spp->setActives();
			}
		}
	}
}
