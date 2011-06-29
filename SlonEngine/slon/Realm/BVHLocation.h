#ifndef __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__

#ifdef _DEBUG
//#define DEBUG_DBVT_LOCATION
#endif

#ifdef DEBUG_DBVT_LOCATION
#   include "../Graphics/Renderable/DebugMesh.h"
#endif
#include "../Utility/Algorithm/aabb_tree.hpp"
#include "Location.h"
#include "BVHLocationNode.h"

namespace slon {
namespace realm {

template<typename Visitor>
class visit_node_functor
{
public:
	visit_node_functor(Visitor& nv_)
	:	nv(nv_)
	{}

	bool operator () (const bvh_location_node_ptr& node) 
	{ 
		nv.traverse( *node->getChild() ); 
		return true;
	}

private:
	Visitor& nv;
};

template<typename Visitor>
visit_node_functor<Visitor> visit_node(Visitor& nv)
{
	return visit_node_functor<Visitor>(nv);
}

class BVHLocation :
    public Location
{
public:
    typedef aabb_tree<bvh_location_node_ptr>    object_tree;
    typedef object_tree::volume_node            object_tree_node;
    typedef object_tree::iterator               object_tree_iterator;

public:
    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Location
    const math::AABBf& getBounds() const;
	
	template<typename Body>
	void visit(const Body& body, scene::NodeVisitor& nv)
	{
		perform_on_leaves(staticAABBTree, body, visit_node(nv));
		perform_on_leaves(dynamicAABBTree, body, visit_node(nv));
	}
	
	template<typename Body>
	void visit(const Body& body, scene::ConstNodeVisitor& nv) const
	{
		perform_on_leaves(staticAABBTree, body, visit_node(nv));
		perform_on_leaves(dynamicAABBTree, body, visit_node(nv));
	}

    void visit(const body_variant& body, scene::NodeVisitor& nv);
    void visit(const body_variant& body, scene::ConstNodeVisitor& nv) const;
    void visitVisible(const math::Frustumf& frustum, scene::NodeVisitor& nv);
    void visitVisible(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const;

    void add(const scene::node_ptr& node, bool dynamic);
    void update(const scene::node_ptr& node);
    bool remove(const scene::node_ptr& node);

private:
    math::AABBf     aabb;
    object_tree     staticAABBTree;
    object_tree     dynamicAABBTree;

    // debug
#ifdef DEBUG_DBVT_LOCATION
    mutable graphics::debug_mesh_ptr debugMesh;
#endif
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__