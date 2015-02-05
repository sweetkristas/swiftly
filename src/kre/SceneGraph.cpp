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

#include <functional>
#include <map>

#include "asserts.hpp"
#include "SceneGraph.hpp"
#include "SceneNode.hpp"
#include "SceneObject.hpp"

namespace KRE
{
	namespace 
	{
		typedef std::map<std::string, ObjectTypeFunction> SceneObjectFactoryLookupTable;
		SceneObjectFactoryLookupTable& get_object_factory()
		{
			static SceneObjectFactoryLookupTable res;
			return res;
		}

		typedef std::map<std::string, std::function<SceneNodePtr(SceneGraph* sg, const variant&)>> SceneNodeRegistry;
		SceneNodeRegistry& get_scene_node_registry()
		{
			static SceneNodeRegistry res;
			return res;
		}
	}
		
	SceneGraph::SceneGraph(const std::string& name) 
		: name_(name) 
	{
	}

	SceneGraph::~SceneGraph() 
	{
	}

	SceneNodePtr SceneGraph::getRootNode()
	{
		return *graph_.begin();
	}

	void SceneGraph::attachNode(SceneNode* parent, SceneNodePtr node) 
	{
		if(parent == NULL) {
			graph_.insert(graph_.end_child(), node);
			node->notifyNodeAttached(graph_.end_child()->get());
			return;
		}
		the::tree<SceneNodePtr>::pre_iterator it = graph_.begin();
		for(; it != graph_.end(); ++it) {
			if(it->get() == parent) {
				//graph_.insert(it, node);
				graph_.insert_below(it, node);
				node->notifyNodeAttached(parent);
				return;
			}
		}
		ASSERT_LOG(false, "parent node not found when attaching a child node");
	}

	SceneGraphPtr SceneGraph::create(const std::string& name) 
	{
		// Create graph then insert a root node into the tree.
		auto sg = std::make_shared<SceneGraph>(name);
		sg->graph_.insert(sg->graph_.end(), sg->createNode());
		return sg;
	}
	
	SceneNodePtr SceneGraph::createNode(const std::string& node_type, const variant& node)
	{
		auto it = get_scene_node_registry().find(node_type);
		if(node_type.empty()) {
			return std::make_shared<SceneNode>(this);
		}
		ASSERT_LOG(it != get_scene_node_registry().end(), "Couldn't find a node with name '" << node_type << "' to create.");
		return it->second(this, node);
	}

	void SceneGraph::registerFactoryFunction(const std::string& type, std::function<SceneNodePtr(SceneGraph*,const variant&)> create_fn)
	{
		auto it = get_scene_node_registry().find(type);
		if(it != get_scene_node_registry().end()) {
			LOG_WARN("Overwriting the Scene Node Function: " << type);
		}
		get_scene_node_registry()[type] = create_fn;
		
	}

	/*SceneObjectPtr SceneGraph::createObject(const std::string& type, const std::string& name) 
	{
		auto it = get_object_factory().find(type);
		ASSERT_LOG(it == get_object_factory().end(), "Couldn't find a way to create the following type of object: " << type);
		return it->second(name);
	}*/

	void SceneGraph::registerObjectType(const std::string& type, ObjectTypeFunction fn)
	{
		auto it = get_object_factory().find(type);
		ASSERT_LOG(it != get_object_factory().end(), "Type(" << type << ") already registered");
		get_object_factory()[type] = fn;
	}

	void SceneGraph::renderSceneHelper(const RenderManagerPtr& renderer, 
		the::tree<SceneNodePtr>::pre_iterator& it, 
		SceneNodeParams* snp)
	{
		if(it == graph_.end()) {
			return;
		}
		// XXX the logic isn't quite right here, snp needs to be cleared at some point.
		(*it)->renderNode(renderer, snp);
		renderSceneHelper(renderer, ++it, snp);
	}

	void SceneGraph::renderScene(const RenderManagerPtr& renderer)
	{
		the::tree<SceneNodePtr>::pre_iterator it = graph_.begin();
		//LOG_DEBUG("RenderScene: " << (*it)->NodeName());
		SceneNodeParams snp;
		renderSceneHelper(renderer, it, &snp);
	}

	void SceneGraph::process(double elapsed_time)
	{
		the::tree<SceneNodePtr>::pre_iterator it = graph_.begin();
		for(; it != graph_.end(); ++it) {
			(*it)->process(elapsed_time);
		}
	}

	std::ostream& operator<<(std::ostream& os, const SceneGraph& sg)
	{
		os << "SCENEGRAPH(";
		the::tree<SceneNodePtr>::const_pre_iterator it = sg.graph_.begin();
		for(; it != sg.graph_.end(); ++it) {
			os << *(*it);
		}
		os << ")\n";
		return os;
	}
}
