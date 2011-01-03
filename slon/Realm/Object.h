#ifndef __SLON_ENGINE_REALM_OBJECT_H__
#define __SLON_ENGINE_REALM_OBJECT_H__

#include "../Config.h"
#include "../Scene/Node.h"
#include "../Utility/Algorithm/spatial_node.hpp"
#include <sgl/Math/AABB.hpp>

#ifdef SLON_ENGINE_USE_PHYSICS
#include "../Physics/PhysicsModel.h"
#endif

namespace slon {
namespace realm {

// forward
class Location;
class World;

/** Single scene object */
class Object :
    public Referenced
{
friend class object_core_access;
private:
    // noncopyable
    Object(const Object&);
    Object& operator = (const Object&);

protected:
    Object() {}

public:
    /** Check wether object is dynamic */
    virtual bool isDynamic() const = 0;

    /** Get AABB of the object */
    virtual const math::AABBf& getBounds() const = 0;

    /** Get root node of the object scene graph */
    virtual const scene::Node* getRoot() const = 0;

    /** Traverse object scene graph */
    virtual void traverse(scene::NodeVisitor& nv) = 0;

    /** Get location where object is located */
    virtual const Location* getLocation() const { return location; }

    /** Get world where object is located */
    virtual const World* getWorld() const { return world; }

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Get physics model of the object. */
    const physics::PhysicsModel* getPhysicsModel() const;
#endif

    virtual ~Object() {}

protected:
    // spatial structure
    World*          world;
    Location*       location;
    spatial_node*   spatialNode; // place binary hacks here

    // node
    scene::node_ptr root;
};

// access to the object fields
class object_core_access
{
public:
    static void             set_world(Object& co, World* world)                     { co.world = world; }
    static void             set_location(Object& co, Location* location)            { co.location = location; }
    static void             set_spatial_node(Object& co, spatial_node* spatialNode) { co.spatialNode = spatialNode; }
    static World*           get_world(Object& co)                                   { return co.world; }
    static Location*        get_location(Object& co)                                { return co.location; }
    static spatial_node*    get_spatial_node(Object& co)                            { return co.spatialNode; }
    static scene::Node*     get_node(Object& co)                                    { return co.root.get(); }

    template<typename spatial_node_t>
    static spatial_node_t*  get_spatial_node(Object& co)                            { return static_cast<spatial_node_t*>(co.spatialNode); }
};

typedef boost::intrusive_ptr<Object>          object_ptr;
typedef boost::intrusive_ptr<const Object>    const_object_ptr;

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_OBJECT_H__
