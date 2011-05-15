#ifndef __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__

#ifdef _DEBUG
//#define DEBUG_DBVT_LOCATION
#endif

#ifdef DEBUG_DBVT_LOCATION
#   include "../../Graphics/Renderable/DebugMesh.h"
#endif
#include "../../Utility/Algorithm/aabb_tree.hpp"
#include "../Location.h"
#include "Object.h"

namespace slon {
namespace realm {
namespace detail {

template<typename Callback>
class object_callback_wrapper
{
public:
	object_callback_wrapper(Callback& cb_)
	:	cb(cb_)
	{}

	bool operator () (const object_ptr& object) { return cb(*object); }

private:
	Callback& cb;
};

template<typename Callback>
object_callback_wrapper<Callback> wrap_callback(Callback& cb)
{
	return object_callback_wrapper<Callback>(cb);
}

class BVHLocation :
    public Location
{
public:
    typedef aabb_tree<object_ptr>       object_tree;
    typedef object_tree::volume_node    object_tree_node;

public:
    // Override Location
    const math::AABBf& getBounds() const;
	
	template<typename Body>
	void visit(const Body& body, object_callback& cb)
	{
		perform_on_leaves(staticAABBTree, body, wrap_callback(cb));
		perform_on_leaves(dynamicAABBTree, body, wrap_callback(cb));
	}
	
	template<typename Body>
	void visit(const Body& body, object_const_callback& cb) const
	{
		perform_on_leaves(staticAABBTree, body, wrap_callback(cb));
		perform_on_leaves(dynamicAABBTree, body, wrap_callback(cb));
	}

    void visit(const body_variant& body, object_callback& cb);
    void visit(const body_variant& body, object_const_callback& cb) const;
    void visitVisible(const math::Frustumf& frustum, object_callback& cb);
    void visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const;

    bool update(realm::Object* object);
    bool remove(realm::Object* object);
    void add(realm::Object* object);

private:
    math::AABBf     aabb;
    object_tree     staticAABBTree;
    object_tree     dynamicAABBTree;

    // debug
#ifdef DEBUG_DBVT_LOCATION
    mutable graphics::debug_mesh_ptr    debugMesh;
    mutable object_ptr                  debugObject;
#endif
};

} // namespace detail
} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__