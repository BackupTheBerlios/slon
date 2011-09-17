#ifndef __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__

#ifdef _DEBUG
//#   define DEBUG_DBVT_LOCATION
#endif

#ifdef DEBUG_DBVT_LOCATION
#   include "../Graphics/DebugMesh.h"
#endif
#include "Location.h"
#include "BVHLocationNode.h"
#include "EventVisitor.h"

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
		return false;
	}

private:
	Visitor& nv;
};

template<typename Visitor>
visit_node_functor<Visitor> visit_node(Visitor& nv)
{
	return visit_node_functor<Visitor>(nv);
}

class SLON_PUBLIC BVHLocation :
    public Location
{
public:
    typedef aabb_tree<bvh_location_node_ptr>    object_tree;
    typedef object_tree::volume_node            object_tree_node;
    typedef object_tree::iterator               object_tree_iterator;

public:
    BVHLocation();
    ~BVHLocation();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Location
    const math::AABBf& getBounds() const;
	
	template<typename Body>
	void visit(const Body& body, scene::Visitor& nv)
	{
		perform_on_leaves(staticAABBTree, body, visit_node(nv));
		perform_on_leaves(dynamicAABBTree, body, visit_node(nv));
	}
	
	template<typename Body>
	void visit(const Body& body, scene::ConstVisitor& nv) const
	{
		perform_on_leaves(staticAABBTree, body, visit_node(nv));
		perform_on_leaves(dynamicAABBTree, body, visit_node(nv));
	}

    void visit(scene::Visitor& nv);
    void visit(scene::ConstVisitor& nv);
    void visit(const body_variant& body, scene::Visitor& nv);
    void visit(const body_variant& body, scene::ConstVisitor& nv) const;
    void visitVisible(const math::Frustumf& frustum, scene::Visitor& nv);
    void visitVisible(const math::Frustumf& frustum, scene::ConstVisitor& nv) const;
	
	bool have(const scene::node_ptr& node) const;
    void add(const scene::node_ptr& node, bool dynamic, bool activatePhysics);
    void update(const scene::node_ptr& node);
    bool remove(const scene::node_ptr& node, bool deactivatePhysics);

    void                          setDynamicsWorld(const physics::dynamics_world_ptr& world);
    physics::DynamicsWorld*       getDynamicsWorld()        { return dynamicsWorld.get(); }
    const physics::DynamicsWorld* getDynamicsWorld() const  { return dynamicsWorld.get(); }

private:
    math::AABBf                 aabb;
    object_tree                 staticAABBTree;
    object_tree                 dynamicAABBTree;
    physics::dynamics_world_ptr dynamicsWorld;
    EventVisitor                eventVisitor;

    // debug
#ifdef DEBUG_DBVT_LOCATION
    mutable graphics::debug_mesh_ptr debugMesh;
#endif
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__