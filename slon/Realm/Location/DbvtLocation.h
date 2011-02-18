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

namespace slon {
namespace realm {

class DbvtLocation :
    public Location
{
public:
    typedef aabb_tree<object_ptr>       object_tree;
    typedef object_tree::volume_node    object_tree_node;

    typedef std::vector<Object*>        object_vector;
    typedef object_vector::iterator     object_iterator;

public:
    // Override Location
    STATE              getState() const  { return ACTIVE; }
    const math::AABBf& getBounds() const;
	
    void visit(const math::Ray3f& ray, scene::NodeVisitor& nv);
    void visit(const math::Ray3f& ray, scene::ConstNodeVisitor& nv) const;
    void visit(const math::AABBf& aabb, scene::NodeVisitor& nv);
    void visit(const math::AABBf& aabb, scene::ConstNodeVisitor& nv) const;
    void visit(const math::Frustumf& frustum, scene::NodeVisitor& nv);
    void visit(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const;

    bool update(Object* object);
    bool remove(Object* object);
    void add(Object* object);

private:
    math::AABBf     aabb;
    object_tree     staticAABBTree;
    object_tree     dynamicAABBTree;

    // debug
#ifdef DEBUG_DBVT_LOCATION
    graphics::debug_mesh_ptr  aabbTreeDebugMesh;
#endif
};

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_LOCATION_DBVT_LOCATION_H__