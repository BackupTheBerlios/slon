#include "stdafx.h"
#include "Database/Detail/UtilitySerialization.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include "Realm/Detail/BVHLocation.h"
#include "Realm/World.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Utility/math.hpp"

namespace {

    using namespace slon;
    using namespace slon::realm::detail;

    template<typename LeafData, typename RealType>
    graphics::DebugMesh* DebugAABBTree(const aabb_tree<LeafData, RealType>& aabbTree)
    {
        using namespace graphics;
        using namespace debug;

        typedef BVHLocation::object_tree		aabb_tree;
        typedef typename aabb_tree::volume_node volume_node;

        DebugMesh* debugMesh = new graphics::DebugMesh();
        *debugMesh << color(1.0f, 1.0f, 0.0f, 1.0f);

        if ( const volume_node* root = aabbTree.getRoot() )
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
namespace detail {

#ifdef DEBUG_DBVT_LOCATION
#   define DEBUG_UPDATE_TREE(debugMesh, staticTree, dynamicTree)\
        {\
            using namespace graphics::debug;\
            if (!debugMesh) {\
                debugMesh.reset(new graphics::DebugMesh);\
            }\
            debugMesh->clear();\
            graphics::debug_mesh_ptr staticDebugMesh( DebugAABBTree(staticTree) );\
            graphics::debug_mesh_ptr dynamicDebugMesh( DebugAABBTree(dynamicTree) );\
            *debugMesh << *staticDebugMesh << *dynamicDebugMesh;\
        }

#   define DEBUG_VISIT_TREE(debugObject, debugMesh, cb)\
        if (!debugObject) {\
            debugObject.reset(new Object(debugMesh.get()));\
        }\
        cb(*debugObject);
#else
#   define DEBUG_UPDATE_TREE(debugMesh, staticTree, dynamicTree)
#   define DEBUG_VISIT_TREE(debugObject, debugMesh, cb)
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
LocationVisitor<Location, Callback> makeLocationVisitor(Location& location, Visitor& visitor)
{
	return LocationVisitor<Location, Callback>(location, visitor);
}

const char* BVHLocation::serialize(database::OArchive& ar) const
{
    struct write_object
    {
        void operator () (database::OArchive& ar, const node_ptr& obj)
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
        scene::node_ptr operator () (database::IArchive& ar)
        {
            return scene::node_ptr(static_cast<scene::Node*>(ar.readSerializable()));
        }
    };

    database::deserialize(ar, "aabb", aabb);
    database::deserialize(ar, "staticAABBTree", staticAABBTree, read_object() );
    database::deserialize(ar, "dynamicAABBTree", dynamicAABBTree, read_object() );
}

const math::AABBf& BVHLocation::getBounds() const
{
    return aabb;
}

void BVHLocation::visit(const body_variant& body, NodeVisitor& nv)
{
	boost::apply_visitor(makeLocationVisitor(*this, nv), body);
}

void BVHLocation::visit(const body_variant& body, ConstNodeVisitor& nv) const
{
	boost::apply_visitor(makeLocationVisitor(*this, nv), body);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, NodeVisitor& nv)
{
	visit(frustum, nv);
    DEBUG_VISIT_TREE(debugObject, debugMesh, nv);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, ConstNodeVisitor& nv) const
{
	visit(frustum, nv);
    DEBUG_VISIT_TREE(debugObject, debugMesh, nv);
}

bool BVHLocation::update(const scene::Node& node)
{
    if (node.getLocation() != this) {
        return false;
    }

    object_tree::iterator bvhIter( reinterpret_cast<object_tree_node*>( node.getLocationData() ) );
    assert(bvhIter);

	scene::TransformVisitor visitor(node);

    if ( node.isDynamic() ) {
        dynamicAABBTree.update(bvhIter, visitor.getBounds());
    }
    else {
        staticAABBTree.update(bvhIter, visitor.getBounds());
    }
    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );

    DEBUG_UPDATE_TREE(debugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

bool BVHLocation::remove(const scene::node_ptr& node)
{
    if (node->getLocation() != this) {
        return false;
    }

    object_tree::iterator bvhIter( reinterpret_cast<object_tree_node*>( node->getLocationData() ) );
    assert(bvhIter);

    node->setLocation(0);
    if ( node->isDynamic() ) {
        dynamicAABBTree.remove(bvhIter);
    }
    else {
        staticAABBTree.remove(bvhIter);
    }
    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );

    DEBUG_UPDATE_TREE(debugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

void BVHLocation::add(const scene::node_ptr& node, bool dynamic)
{
	assert(node);
	if ( Location* location = node->getLocation() ) {
		location->remove(node);
	}
	
    // recompute aabb
    scene::TransformVisitor visitor(*node);
    if (dynamic) {
        node->setLocationData( dynamicAABBTree.insert(visitor.getBounds(), node).get_node() );
    }
    else {
        node->setLocationData( staticAABBTree.insert(visitor.getBounds(), node).get_node() );
    }
    node->setLocation(this);
    aabb = math::merge( staticAABBTree.get_bounds(), dynamicAABBTree.get_bounds() );

    DEBUG_UPDATE_TREE(debugMesh, staticAABBTree, dynamicAABBTree)
}

} // namespace detail
} // namespace realm
} // namespace slon
