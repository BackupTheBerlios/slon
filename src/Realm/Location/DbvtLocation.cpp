#include "stdafx.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include "Realm/Location/DbvtLocation.h"
#include "Realm/World.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Utility/math.hpp"

namespace {

    using namespace slon;
    using namespace slon::realm;

    template<typename NodeVisitor>
    struct update_static_object_functor
    {
        update_static_object_functor(NodeVisitor& _nv) :
            nv(_nv)
        {}

        void operator () (const object_ptr& object) { object->traverse(nv); }

        NodeVisitor& nv;
    };

    template<typename NodeVisitor>
    struct update_dynamic_object_functor
    {
        update_dynamic_object_functor(NodeVisitor& _nv) :
            nv(_nv)
        {}

        void operator () (const object_ptr& object) { object->traverse(nv); }

        NodeVisitor& nv;
    };

    // update object bounds when traversing
    template<>
    struct update_dynamic_object_functor<scene::TraverseVisitor>
    {
        update_dynamic_object_functor(scene::TraverseVisitor& _nv) :
            nv(_nv)
        {}

        void operator () (const object_ptr& object)
        {
            math::AABBf oldBounds = object->getBounds();
            object->traverse(nv);
            if ( oldBounds != object->getBounds() ) {
                object_core_access::get_world(*object)->update( object.get() );
            }
        }

        scene::TraverseVisitor& nv;
    };

    template<typename NodeVisitor>
    update_static_object_functor<NodeVisitor> update_static_object(NodeVisitor& nv)
    {
        return update_static_object_functor<NodeVisitor>(nv);
    }

    template<typename NodeVisitor>
    update_dynamic_object_functor<NodeVisitor> update_dynamic_object(NodeVisitor& nv)
    {
        return update_dynamic_object_functor<NodeVisitor>(nv);
    }

    template<typename Volume>
    inline void traverse_static( DbvtLocation::object_tree&     aabbTree,
                                 const Volume&                  volume,
                                 scene::NodeVisitor&            nv )
    {
        perform_on_leaves( aabbTree, volume, update_static_object(nv) );
    }

    template<typename Volume>
    inline void traverse_dynamic( DbvtLocation::object_tree&     aabbTree,
                                  const Volume&                  volume,
                                  scene::NodeVisitor&            nv )
    {
        if ( scene::TraverseVisitor* tv = dynamic_cast<scene::TraverseVisitor*>(&nv) ) {
            perform_on_leaves( aabbTree, volume, update_dynamic_object(*tv) );
        }
        else {
            perform_on_leaves( aabbTree, volume, update_dynamic_object(nv) );
        }
    }

    template<typename LeafData, typename RealType>
    graphics::DebugMesh* DebugAABBTree(const aabb_tree<LeafData, RealType>& aabbTree)
    {
        using namespace graphics;
        using namespace debug;

        typedef DbvtLocation::object_tree		aabb_tree;
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

const math::AABBf& DbvtLocation::getBounds() const
{
    return aabb;
}

void DbvtLocation::visit(const math::Ray3f& ray, scene::NodeVisitor& nv)
{
    traverse_static(staticAABBTree,   ray, nv);
    traverse_dynamic(dynamicAABBTree, ray, nv);
    DEBUG_TRAVERSE_TREE(aabbTreeDebugMesh, nv);
}

void DbvtLocation::visit(const math::AABBf& aabb, scene::NodeVisitor& nv)
{
    traverse_static(staticAABBTree,   aabb, nv);
    traverse_dynamic(dynamicAABBTree, aabb, nv);
    DEBUG_TRAVERSE_TREE(aabbTreeDebugMesh, nv);
}

void DbvtLocation::visit(const math::Frustumf& frustum, scene::NodeVisitor& nv)
{
    traverse_static(staticAABBTree,   frustum, nv);
    traverse_dynamic(dynamicAABBTree, frustum, nv);
    DEBUG_TRAVERSE_TREE(aabbTreeDebugMesh, nv);
}

bool DbvtLocation::update(Object* object)
{
    assert(object);
    if (object_core_access::get_location(*object) != this) {
        return false;
    }
    assert( object_core_access::get_spatial_node(*object) );

    if ( object->isDynamic() ) {
        dynamicAABBTree.update( object->getBounds(), object_core_access::get_spatial_node<object_tree_node>(*object) );
    }
    else {
        staticAABBTree.update( object->getBounds(), object_core_access::get_spatial_node<object_tree_node>(*object) );
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

bool DbvtLocation::remove(Object* object)
{
    assert(object);
    if (object_core_access::get_location(*object) != this) {
        return false;
    }
    assert( object_core_access::get_spatial_node(*object) );

    object_core_access::set_location(*object, 0);
    if ( object->isDynamic() ) {
        dynamicAABBTree.remove( object_core_access::get_spatial_node<object_tree_node>(*object) );
    }
    else {
        staticAABBTree.remove( object_core_access::get_spatial_node<object_tree_node>(*object) );
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
    return true;
}

void DbvtLocation::add(Object* object)
{
    assert(object && object_core_access::get_location(*object) == 0);

    object_core_access::set_location(*object, this);
    if ( object->isDynamic() ) 
    {
        // recompute aabb
        scene::TraverseVisitor visitor;
        object->traverse(visitor);

        // insert into tree
        object_core_access::set_spatial_node( *object, dynamicAABBTree.insert( object->getBounds(), object_ptr(object) ) );
    }
    else {
        object_core_access::set_spatial_node( *object, staticAABBTree.insert( object->getBounds(), object_ptr(object) ) );
    }
    aabb = math::merge( staticAABBTree.getBounds(), dynamicAABBTree.getBounds() );

    DEBUG_UPDATE_TREE(aabbTreeDebugMesh, staticAABBTree, dynamicAABBTree)
}

} // namesapce realm
} // namespace slon
