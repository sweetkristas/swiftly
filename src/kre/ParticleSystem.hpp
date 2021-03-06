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
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	   3. This notice may not be removed or altered from any source
	   distribution.
*/

#pragma once

#include <memory>
#include <random>
#include <sstream>
#include <glm/glm.hpp>

#include "asserts.hpp"
#include "AttributeSet.hpp"
#include "Material.hpp"
#include "ParticleSystemFwd.hpp"
#include "SceneNode.hpp"
#include "SceneObject.hpp"
#include "SceneUtil.hpp"

namespace KRE
{
	namespace Particles
	{
		typedef glm::detail::tvec4<unsigned char> color_vector;

		struct vertex_texture_color3
		{
			vertex_texture_color3(const glm::vec3& v, const glm::vec2& t, const glm::u8vec4& c)
				: vertex(v), texcoord(t), color(c) {}
			glm::vec3 vertex;
			glm::vec2 texcoord;
			glm::u8vec4 color;
		};

		struct PhysicsParameters
		{
			glm::vec3 position;
			color_vector color;
			glm::vec3 dimensions;
			float time_to_live;
			float mass;
			float velocity;
			glm::vec3 direction;
			glm::quat orientation;
		};

		void init_physics_parameters(PhysicsParameters& pp); 

		// This structure should be POD (i.e. plain old data)
		struct Particle
		{
			PhysicsParameters current;
			PhysicsParameters initial;
			Emitter* emitted_by;
		};

		// General class for emitter objects which encapsulate and exposes physical parameters
		// Used as a base class for everything that is not 
		class EmitObject : public Particle
		{
		public:
			explicit EmitObject(ParticleSystemContainer* parent, const variant& node) 
				: parent_container_(parent) {
				ASSERT_LOG(parent != NULL, "PSYSTEM2: parent is null");
				if(node.has_key("name")) {
					name_ = node["name"].as_string();
				} else {
					std::stringstream ss;
					ss << "emit_object_" << int(get_random_float());
					name_ = ss.str();
				}
			}
			virtual ~EmitObject() {}
			const std::string& name() const { return name_; }
			void emitProcess(float t) {
				handleEmitProcess(t);
			}
			void draw() const {
				handleDraw();
			}
			ParticleSystemContainer* getParentContainer() { 
				ASSERT_LOG(parent_container_ != NULL, "PSYSTEM2: parent container is NULL");
				return parent_container_; 
			}
			virtual const glm::vec3& getPosition() const;
			virtual void setPosition(const glm::vec3& pos) {}
		protected:
			virtual bool durationExpired() { return false; }
		private:
			virtual void handleEmitProcess(float t) = 0;
			virtual void handleDraw() const {}
			std::string name_;
			ParticleSystemContainer* parent_container_;

			EmitObject();
		};

		class Technique  : public EmitObject, public SceneObject
		{
		public:
			explicit Technique(ParticleSystemContainer* parent, const variant& node);
			Technique(const Technique& tq);
			virtual ~Technique();

			size_t getParticleCount() const { return active_particles_.size(); };
			size_t getQuota() const { return particle_quota_; }
			size_t getEmitterQuota() const { return emitter_quota_; }
			size_t getSystemQuota() const { return system_quota_; }
			size_t getTechniqueQuota() const { return technique_quota_; }
			size_t getAffectorQuota() const { return affector_quota_; }
			glm::vec3 getDefaultDimensions() const { return glm::vec3(default_particle_width_, default_particle_height_, default_particle_depth_); }
			ParticleSystem* getParticleSystem() { return particle_system_; }
			EmitObjectPtr getEmitObject(const std::string& name);
			void setParent(ParticleSystem* parent);
			// Direct access here for *speed* reasons.
			std::vector<Particle>& getActiveParticles() { return active_particles_; }
			std::vector<EmitterPtr>& getInstancedEmitters() { return instanced_emitters_; }
			std::vector<AffectorPtr>& getInstancedAffectors() { return instanced_affectors_; }
			void addEmitter(EmitterPtr e);
			void addAffector(AffectorPtr a);
			void preRender(const WindowManagerPtr& wnd) override;
		private:
			void init();
			void handleEmitProcess(float t) override;
			void doAttach(const DisplayDevicePtr& dd, DisplayDeviceDef* def) override;

			std::shared_ptr<Attribute<vertex_texture_color3>> arv_;

			float default_particle_width_;
			float default_particle_height_;
			float default_particle_depth_;
			size_t particle_quota_;
			size_t emitter_quota_;
			size_t affector_quota_;
			size_t technique_quota_;
			size_t system_quota_;
			int lod_index_;
			float velocity_;
			std::unique_ptr<float> max_velocity_;

			//renderer_ptr renderer_;
			std::vector<EmitterPtr> active_emitters_;
			std::vector<AffectorPtr> active_affectors_;

			std::vector<EmitterPtr> instanced_emitters_;
			std::vector<AffectorPtr> instanced_affectors_;

			// Parent particle system
			ParticleSystem* particle_system_;

			// List of particles currently active.
			std::vector<Particle> active_particles_;

			Technique();
		};

		class ParticleSystem : public EmitObject, public SceneNode
		{
		public:
			explicit ParticleSystem(SceneGraph* sg, ParticleSystemContainer* parent, const variant& node);
			ParticleSystem(const ParticleSystem& ps);
			virtual ~ParticleSystem();

			float getElapsedTime() const { return elapsed_time_; }
			float getScaleVelocity() const { return scale_velocity_; }
			float getScaleTime() const { return scale_time_; }
			const glm::vec3& getScaleDimensions() const { return scale_dimensions_; }

			static ParticleSystem* factory(ParticleSystemContainer* parent, const variant& node);

			void addTechnique(TechniquePtr tq);
			std::vector<TechniquePtr>& getActiveTechniques() { return active_techniques_; }

		protected:
			virtual void handleEmitProcess(float t) override;
			void notifyNodeAttached(SceneNode* parent) override;
			void update(float t);

			float elapsed_time_;
			float scale_velocity_;
			float scale_time_;
			glm::vec3 scale_dimensions_;

			std::unique_ptr<std::pair<float,float>> fast_forward_;

			// List of how to create and manipulate particles.
			std::vector<TechniquePtr> active_techniques_;

			ParticleSystem();
		};

		class ParticleSystemContainer : public SceneNode
		{
		public:
			explicit ParticleSystemContainer(SceneGraph* sg, const variant& node);
			virtual ~ParticleSystemContainer();

			void getActivateParticleSystem(const std::string& name);
			std::vector<ParticleSystemPtr>& getActiveParticleSystems() { return active_particle_systems_; }

			ParticleSystemPtr cloneParticleSystem(const std::string& name);
			TechniquePtr cloneTechnique(const std::string& name);
			EmitterPtr cloneEmitter(const std::string& name);
			AffectorPtr cloneAffector(const std::string& name);

			void addParticleSystem(ParticleSystem* obj);
			void addTechnique(Technique* obj);
			void addEmitter(Emitter* obj);
			void addAffector(Affector* obj);

			std::vector<ParticleSystemPtr> cloneParticleSystems();
			std::vector<TechniquePtr> cloneTechniques();
			std::vector<EmitterPtr> cloneEmitters();
			std::vector<AffectorPtr> cloneAffectors();

			void process(double current_time) override;
		private:
			void notifyNodeAttached(SceneNode* parent) override;
			std::vector<ParticleSystemPtr> active_particle_systems_;

			std::vector<ParticleSystemPtr> particle_systems_;
			std::vector<TechniquePtr> techniques_;
			std::vector<EmitterPtr> emitters_;
			std::vector<AffectorPtr> affectors_;
			
			ParticleSystemContainer();
			ParticleSystemContainer(const ParticleSystemContainer&);
		};

		std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
		std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
		std::ostream& operator<<(std::ostream& os, const glm::quat& v);

		glm::vec3 perpendicular(const glm::vec3& v) ;
		glm::vec3 create_deviating_vector(float angle, const glm::vec3& v, const glm::vec3& up = glm::vec3(0.0f));
	}
}
