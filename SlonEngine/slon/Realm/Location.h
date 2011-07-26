#ifndef __SLON_ENGINE_REALM_LOCATION_H__
#define __SLON_ENGINE_REALM_LOCATION_H__

#include "../Database/Serializable.h"
#include "../Physics/Forward.h"
#include "../Scene/Forward.h"
#include "../Thread/Lock.h"
#include "../Utility/math.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace realm {
	
class Location :
    public Referenced,
    public database::Serializable
{
public:
    /** Get bounds of the hole location. */
    virtual const math::AABBf& getBounds() const = 0;

	/** Check whether location have specified node. */
	virtual bool have(const scene::node_ptr& node) const = 0;

    /** Add object to the world. Doesn't check for duplicates. 
	 * @param node - scene graph node for insertion (must be root). Some location classes may add parent
     * nodes for scene graph to store necessary information in them.
	 * @param dynamic - hint, object transform will be frequently updated during execution.
     * @param activatePhysics - add node physics objects to the dynamics world.
	 */
    virtual void add(const scene::node_ptr& node, 
                     bool                   dynamic = true,
                     bool                   activatePhysics = true) = 0;
		
    /** Update spatial structure for the object. Generally you haven't to call this
     * function, engine handles it automatically.
     */
    virtual void update(const scene::node_ptr& node) = 0;

    /** Remove object from the location if it is presented. 
	 * @param node - scene graph node for removal (same as used in add function).
     * @param deactivatePhysics - remove node physics objects from the dynamics world.
     * @return true if object removed
     */
    virtual bool remove(const scene::node_ptr& node,
                        bool                   deactivatePhysics = true) = 0;

    /** Visit location nodes. */
    virtual void visit(scene::Visitor& nv) = 0;

    /** Visit location nodes. */
    virtual void visit(scene::ConstVisitor& nv) = 0;

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

    /** Set dynamics world for location. Physics entities will be added to dynamics world. */
    virtual void setDynamicsWorld(const physics::dynamics_world_ptr& world) = 0;

    /** Get location dynamics world. */
    virtual physics::DynamicsWorld* getDynamicsWorld() = 0;

    /** Get location dynamics world. */
    virtual const physics::DynamicsWorld* getDynamicsWorld() const = 0;


    /** Grant thread read access to the location.
     * @return lock object. Lock is freed whether object is deleted.
     *
    virtual thread::lock_ptr lockForReading() const = 0;*/

    /** Grant thread write access to the location.
     * @return lock object. Lock is freed whether object is deleted.
     *
    virtual thread::lock_ptr lockForWriting() = 0;*/

    virtual ~Location() {}
};

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_LOCATION_H__
