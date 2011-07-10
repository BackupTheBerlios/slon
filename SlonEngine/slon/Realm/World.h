#ifndef __SLON_ENGINE_REALM_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_H__

#include "../Database/Serializable.h"
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
    public Referenced,
    public database::Serializable
{
public:
    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, scene::Visitor& nv) = 0;

    /** Visit objects intersecting body.
     * @param body - body which intersects objects.
     * @param cb - visitor.
     */
    virtual void visit(const body_variant& body, scene::ConstVisitor& nv) const = 0;
	
    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, scene::Visitor& nv) = 0;

    /** Visit objects visible in frustum.
     * @param frustum - frustum which intersects objects.
     * @param cb - visitor.
     */
    virtual void visitVisible(const math::Frustumf& frustum, scene::ConstVisitor& nv) const = 0;
	   
	/** Remove infinite object from the world if it is presented. 
     * @return true if object removed
     */
    virtual bool removeInfiniteNode(const scene::node_ptr& object) = 0;

    /** Add infinite object to the world. Doesn't check for duplicates. */
    virtual void addInfiniteNode(const scene::node_ptr& object) = 0;
	
    /** Check whether world have specified node. */
	virtual bool haveInfiniteNode(const scene::node_ptr& node) const = 0;

    /** Add location to the world */
    virtual void addLocation(const location_ptr& location) = 0;

    /** Remove location from the world.
     * @return true - if succeeds, false if location not found
     */
    virtual bool removeLocation(const location_ptr& location) = 0;

    /** Check whether world have specified location */
    virtual bool haveLocation(const location_ptr& location) const = 0;

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
World* currentWorld();

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_H__