#include "stdafx.h"
#include "Database/Detail/UtilitySerialization.h"
#include "Graphics/DebugDrawCommon.h"
#include "Physics/DynamicsWorld.h"
#include "Realm/BVHLocation.h"
#include "Realm/World.h"
#include "Scene/TransformVisitor.h"
#include "Utility/math.hpp"

namespace {

    using namespace slon;
    using namespace slon::realm;

    template<typename LeafData, typename RealType>
    graphics::DebugMesh* DebugAABBTree(const aabb_tree<LeafData, RealType>& aabbTree)
    {
        using namespace graphics;
        using namespace debug;

        typedef BVHLocation::object_tree		aabb_tree;
        typedef typename aabb_tree::volume_node volume_node;

        DebugMesh* debugMesh = new graphics::DebugMesh();
        *debugMesh << color(1.0f, 1.0f, 0.0f, 1.0f);

        if ( const volume_node* root = aabbTree.get_root() )
        {
            std::queue<const volume_node*> queue;
            queue.push(root);

            while ( !queue.empty() )
            {
                root = queue.front();
                queue.pop();

                *debugMesh << root->get_bounds();
                if ( root->is_internal() )
                {
                    queue.push( root->get_child(0) );
                    queue.push( root->get_child(1) );
                }
            }
        }

        return debugMesh;
    }

}

namespace slon {
namespace realm {

#ifdef DEBUG_DBVT_LOCATION
#   define DEBUG_UPDATE_TREE(debugMesh, aabb, staticTree, dynamicTree)\
        {\
            using namespace graphics::debug;\
            if (!debugMesh) {\
                debugMesh.reset(new graphics::DebugMesh);\
            }\
            debugMesh->clear();\
            graphics::debug_mesh_ptr staticDebugMesh( DebugAABBTree(staticTree) );\
            graphics::debug_mesh_ptr dynamicDebugMesh( DebugAABBTree(dynamicTree) );\
            *debugMesh << aabb << *staticDebugMesh << *dynamicDebugMesh;\
        }

#   define DEBUG_VISIT_TREE(debugMesh, cv)\
        cv.traverse(*debugMesh);
#else
#   define DEBUG_UPDATE_TREE(debugMesh, aabb, staticTree, dynamicTree)
#   define DEBUG_VISIT_TREE(debugMesh, cv)
#endif
	
template<typename Location, typename Visitor>
class LocationVisitor : 
	public boost::static_visitor<void>
{
public:
	LocationVisitor(Location& location_, Visitor& visitor_)
	:	location(location_)
	,	visitor(visitor_)
	{}

	void operator () (const math::Ray3f& body) const
	{
        location.visit(body, visitor);
	}
    
	void operator () (const math::Sphere3f& body) const
	{
		//location.visit<math::Sphere3f>(body, cb);
	}

	void operator () (const math::AABBf& body) const
	{
        location.visit(body, visitor);
	}
		
	void operator () (const math::Frustumf& body) const
	{
        location.visit(body, visitor);
	}

private:
	Location& location;
	Visitor&  visitor;
};

template<typename Location, typename Visitor>
LocationVisitor<Location, Visitor> makeLocationVisitor(Location& location, Visitor& visitor)
{
	return LocationVisitor<Location, Visitor>(location, visitor);
}

BVHLocation::BVHLocation()
{
    eventVisitor.setLocation(this);
}

const char* BVHLocation::serialize(database::OArchive& ar) const
{
    struct write_object
    {
        void operator () (database::OArchive& ar, const bvh_location_node_ptr& obj)
        {
            ar.writeSerializable(obj.get());
        }
    };

    database::serialize(ar, "aabb", aabb);
    database::serialize(ar, "staticAABBTree", staticAABBTree, write_object() );
    database::serialize(ar, "dynamicAABBTree", dynamicAABBTree, write_object() );

    return "BVHLocation";
}

void BVHLocation::deserialize(database::IArchive& ar)
{
    struct read_object
    {
        bvh_location_node_ptr operator () (database::IArchive& ar)
        {
            return bvh_location_node_ptr(ar.readSerializable<BVHLocationNode>());
        }
    };

    database::deserialize(ar, "aabb", aabb);
    database::deserialize(ar, "staticAABBTree", staticAABBTree, read_object() );
    database::deserialize(ar, "dynamicAABBTree", dynamicAABBTree, read_object() );

    for (object_tree_iterator iter  = staticAABBTree.begin(); 
                              iter != staticAABBTree.end();
                              ++iter)
    {
        (*iter)->setBVHIterator(iter);
    }

    for (object_tree_iterator iter  = dynamicAABBTree.begin(); 
                              iter != dynamicAABBTree.end();
                              ++iter)
    {
        (*iter)->setBVHIterator(iter);
    }
}

const math::AABBf& BVHLocation::getBounds() const
{
    return aabb;
}

void BVHLocation::visit(scene::Visitor& nv)
{
    perform_on_leaves(staticAABBTree, visit_node(nv));
}

void BVHLocation::visit(scene::ConstVisitor& nv)
{
    perform_on_leaves(staticAABBTree, visit_node(nv));
}

void BVHLocation::visit(const body_variant& body, scene::Visitor& nv)
{
	boost::apply_visitor(makeLocationVisitor(*this, nv), body);
}

void BVHLocation::visit(const body_variant& body, scene::ConstVisitor& nv) const
{
	boost::apply_visitor(makeLocationVisitor(*this, nv), body);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, scene::Visitor& nv)
{
	visit(frustum, nv);
    DEBUG_VISIT_TREE(debugMesh, nv);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, scene::ConstVisitor& nv) const
{
	visit(frustum, nv);
    DEBUG_VISIT_TREE(debugMesh, nv);
}

void BVHLocation::update(const scene::node_ptr& node)
{
    BVHLocationNode* locNode = static_cast<BVHLocationNode*>( node->getParent() );
    assert(locNode && locNode->getLocation() == this);

	scene::TransformVisitor visitor(*node);
    if ( locNode->isDynamic() ) {
        dynamicAABBTree.update(locNode->getBVHIterator(), visitor.getBounds());
    }
    else {
        staticAABBTree.update(locNode->getBVHIterator(), visitor.getBounds());
    }

    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );
    DEBUG_UPDATE_TREE(debugMesh, aabb, staticAABBTree, dynamicAABBTree);
}

bool BVHLocation::have(const scene::node_ptr& node) const
{
	bvh_location_node_ptr locNode( dynamic_cast<BVHLocationNode*>(node->getParent()) );
	if (!locNode) {
		return false;
	}

	return locNode->getLocation() == this;
}

void BVHLocation::add(const scene::node_ptr& node, bool dynamic)
{
    assert( node && !node->getParent() );
	bvh_location_node_ptr locNode( new BVHLocationNode(this, 0, dynamic) );
    locNode->addChild(node);

    // recompute aabb
    scene::TransformVisitor visitor(*node);
    if (dynamic) {
        locNode->setBVHIterator( dynamicAABBTree.insert(visitor.getBounds(), locNode) );
    }
    else {
        locNode->setBVHIterator( staticAABBTree.insert(visitor.getBounds(), locNode) );
    }

    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );
    DEBUG_UPDATE_TREE(debugMesh, aabb, staticAABBTree, dynamicAABBTree)

    eventVisitor.setType(EventVisitor::WORLD_ADD);
    eventVisitor.traverse(*node);
}

bool BVHLocation::remove(const scene::node_ptr& node)
{
	assert(node);

    bvh_location_node_ptr locNode = static_cast<BVHLocationNode*>( node->getParent() );
    if (!locNode || locNode->getLocation() != this) {
        return false;
    }

    if ( locNode->isDynamic() ) {
        dynamicAABBTree.remove(locNode->getBVHIterator());
    }
    else {
        staticAABBTree.remove(locNode->getBVHIterator());
    }
    locNode->removeChild(node.get());

    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );
    DEBUG_UPDATE_TREE(debugMesh, aabb, staticAABBTree, dynamicAABBTree)
    
    eventVisitor.setType(EventVisitor::WORLD_REMOVE);
    eventVisitor.traverse(*node);
    return true;
}

void BVHLocation::setDynamicsWorld(const physics::dynamics_world_ptr& dynamicsWorld_)
{
    if (dynamicsWorld)
    {
        dynamicsWorld.reset();
        eventVisitor.setType(EventVisitor::WORLD_REMOVE);
        visit(eventVisitor);
    }

    if (dynamicsWorld_)
    {
        dynamicsWorld = dynamicsWorld_;
        eventVisitor.setType(EventVisitor::WORLD_ADD);
        visit(eventVisitor);
    }
}

} // namespace realm
} // namespace slon
