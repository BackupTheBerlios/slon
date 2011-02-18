#ifndef __SLON_ENGINE_REALM_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_H__

#include "../Scene/Visitors/NodeVisitor.h"
#include "../Thread/Lock.h"
#include "Object.h"
#include <sgl/Math/Frustum.hpp>
#include <sgl/Math/Ray.hpp>
//#include <sgl/Math/Sphere.hpp>

namespace slon {
namespace realm {

/** World stores all locations represented in the application.
 * It handles location loading, storing and updating. World is not 
 * thread safe by itself, you must use lock functions to provide thread
 * safety.
 */
class World :
    public Referenced
{
public:
    enum UPDATE_POLICY
    {
        UPDATE_ALL,         /// Update all locations every frame
        UPDATE_ACTIVE,      /// Update all active locations
        UPDATE_ALL_AWAKEN   /// Update all locations which are not sleeping 
    };

    enum EVICTION_POLICY
    {
        NO_EVICTION,        /// Do not evict loaded locations
        EVICT_SLEEPING,     /// Evict locations which are going to sleep   
        EVICT_INACTIVE      /// Evict locations on deactivation
    };

    enum LOAD_POLICY
    {
        LOAD_ON_STARTUP,    /// Load all locations on startup
        LOAD_ON_WAKE,       /// Load locations which ready to wake up
        LOAD_ON_ACTIVATE    /// Load locations which ready to activate
    };

public:
    virtual UPDATE_POLICY getUpdatePolicy() const = 0;

    virtual EVICTION_POLICY getEvictionPolicy() const = 0;

    virtual LOAD_POLICY getLoadPolicy() const = 0;
	
    /** Visit objects intersecting the ray. */
    virtual void visit(const math::Ray3f& ray, scene::NodeVisitor& nv) = 0;

    /** Visit objects intersecting the ray. */
    virtual void visit(const math::Ray3f& ray, scene::ConstNodeVisitor& nv) const = 0;

    /** Visit objects in the specified area. */
    //virtual void visit(const math::Spheref& area, scene::NodeVisitor& nv) const = 0;

    /** Visit objects in the intersecting AABB area. */
    virtual void visit(const math::AABBf& area, scene::NodeVisitor& nv) = 0;

    /** Visit objects in the intersecting AABB area. */
    virtual void visit(const math::AABBf& area, scene::ConstNodeVisitor& nv) const = 0;

    /** Visit objects in the intersecting frustum area. */
    virtual void visit(const math::Frustumf& frustum, scene::NodeVisitor& nv) = 0;

    /** Visit objects in the intersecting frustum area. */
    virtual void visit(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const = 0;

    /** Update object in the spatial structure if it is presented. 
     * @return true if object updated.
     */
    virtual bool update(Object* object) = 0;

    /** Remove object from the world if it is presented. 
     * @return true if object removed
     */
    virtual bool remove(Object* object) = 0;

    /** Add object to the world. Doesn't check for duplicates. */
    virtual void add(Object* object) = 0;

    /** Grant thread read access to the world.
     * @return lock object. Lock is freed wether object is deleted.
     */
    virtual thread::lock_ptr lockForReading() const = 0;

    /** Grant thread write access to the world.
     * @return lock object. Lock is freed wether object is deleted.
     */
    virtual thread::lock_ptr lockForWriting() = 0;

    virtual ~World() {}
};

/** Get simulation world */
World* currentWorld();

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_H__