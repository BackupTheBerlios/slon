#include "stdafx.h"
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

	template<typename Tree, typename Callback>
	class visitor : 
		public boost::static_visitor<void>
	{
	private:
		template<typename T>
		void perform(const T& body)
		{
            perform_on_leaves(tree, body, cb);
		}

	public:
		visitor(Tree& tree_, Callback& cb_)
		:	tree(tree_)
		,	cb(cb_)
		{}

		void operator () (const math::Ray3f& body) const
		{
			perform(body);
		}
    
		void operator () (const math::Sphere3f& body) const
		{
			perform(body);
		}

		void operator () (const math::AABBf& body) const
		{
			perform(body);
		}
		
		void operator () (const math::Frustumf& body) const
		{
			perform(body);
		}

	private:
		Tree&		tree;
		Callback&	cb;
	};

	template<typename Tree, typename Callback>
	visitor<Tree, Callback> make_visitor(Tree& tree, Callback& cb)
	{
        return visitor<Tree, Callback>(tree, cb);
	}

}

namespace slon {
namespace realm {
namespace detail {

#ifdef DEBUG_DBVT_LOCATION
#   define DEBUG_UPDATE_TREE(debugMesh, staticTree, dynamicTree)\
        {\
            using namespace graphics::debug;\
            debugMesh.reset(new graphics::DebugMesh);\
            graphics::debug_mesh_ptr staticDebugMesh( DebugAABBTree(staticTree) );\
            graphics::debug_mesh_ptr dynamicDebugMesh( DebugAABBTree(dynamicTree) );\
            *debugMesh << *staticDebugMesh << *dynamicDebugMesh;\
        }

#   define DEBUG_TRAVERSE_TREE(debugTree, nv)\
        nv.traverse(*debugTree);
#else
#   define DEBUG_UPDATE_TREE(debugMesh, staticTree, dynamicTree)
#   define DEBUG_TRAVERSE_TREE(debugTree, nv)
#endif
	
template<typename Location, typename Callback>
class LocationVisitor : 
	public boost::static_visitor<void>
{
public:
	LocationVisitor(Location& location_, Callback& cb_)
	:	location(location_)
	,	cb(cb_)
	{}

	void operator () (const math::Ray3f& body) const
	{
        location.visit(body, cb);
	}
    
	void operator () (const math::Sphere3f& body) const
	{
		//location.visit<math::Sphere3f>(body, cb);
	}

	void operator () (const math::AABBf& body) const
	{
        location.visit(body, cb);
	}
		
	void operator () (const math::Frustumf& body) const
	{
        location.visit(body, cb);
	}

private:
	Location& location;
	Callback& cb;
};

template<typename Location, typename Callback>
LocationVisitor<Location, Callback> makeLocationVisitor(Location& location, Callback& cb)
{
	return LocationVisitor<Location, Callback>(location, cb);
}

const math::AABBf& BVHLocation::getBounds() const
{
    return aabb;
}

void BVHLocation::visit(const body_variant& body, object_callback& cb)
{
	boost::apply_visitor(makeLocationVisitor(*this, cb), body);
}

void BVHLocation::visit(const body_variant& body, object_const_callback& cb) const
{
	boost::apply_visitor(makeLocationVisitor(*this, cb), body);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, object_callback& cb)
{
	visit(frustum, cb);
}

void BVHLocation::visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const
{
	visit(frustum, cb);
}

bool BVHLocation::update(realm::Object* object_)
{
	detail::Object* object = static_cast<detail::Object*>(object_);
    assert(object);
    if (object->getLocation() != this) {
        return false;
    }

	object_tree_node* bvhNode = reinterpret_cast<object_tree_node*>( object->getLocationData() );
    assert(bvhNode);

    if ( object->isDynamic() ) {
        dynamicAABBTree.update(object->getBounds(), bvhNode);
    }
    else {
        staticAABBTree.update(object->getBounds(), bvhNode);
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

bool BVHLocation::remove(realm::Object* object_)
{
	detail::Object* object = static_cast<detail::Object*>(object_);
    assert(object);
    if (object->getLocation() != this) {
        return false;
    }

	object_tree_node* bvhNode = reinterpret_cast<object_tree_node*>( object->getLocationData() );
    assert(bvhNode);

    object->setLocation(0);
    if ( object->isDynamic() ) {
        dynamicAABBTree.remove(bvhNode);
    }
    else {
        staticAABBTree.remove(bvhNode);
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

void BVHLocation::add(realm::Object* object_)
{
	detail::Object* object = static_cast<detail::Object*>(object_);
	assert(object);

	if ( Location* location = object->getLocation() ) {
		location->remove(object);
	}

    object->setLocation(this);
    if ( object->isDynamic() ) 
    {
        // recompute aabb
        scene::TransformVisitor visitor;
        object->traverse(visitor);

        // insert into tree
        object->setLocationData( dynamicAABBTree.insert( object->getBounds(), object_ptr(object) ) );
    }
    else {
        object->setLocationData( staticAABBTree.insert( object->getBounds(), object_ptr(object) ) );
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
}

} // namespace detail
} // namespace realm
} // namespace slon
