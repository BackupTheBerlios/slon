#ifndef __SLON_ENGINE_REALM_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_H__

#include "../Scene/Forward.h"
#include "../Thread/Lock.h"
#include "../Utility/callback.hpp"
#include "../Utility/math.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "../Physics/Forward.h"
#endif
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
    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, object_callback& cb) = 0;

    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, object_const_callback& cb) const = 0;
	
    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, object_callback& cb) = 0;

    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const = 0;

	/** Create object. Object is not added into world by default. */
	virtual Object* createObject() const = 0;

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
	
	/** Create object and add it into world. */
	virtual realm::Object* add(scene::Node*				node,
					           bool						dynamic = false
#ifdef SLON_ENGINE_USE_PHYSICS
					           , physics::PhysicsModel*	physicsModel = 0
#endif                 
					           ) = 0;

    /** Grant thread read access to the world.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForReading() const = 0;

    /** Grant thread write access to the world.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForWriting() = 0;

    virtual ~World() {}
};

/** Get simulation world */
World& currentWorld();

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_H__