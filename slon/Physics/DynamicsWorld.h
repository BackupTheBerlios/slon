#ifndef __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__
#define __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__

#include "../Thread/Lock.h"
#include "../Utility/referenced.hpp"
#include "Constraint.h"
#include "RigidBody.h"
#include <sgl/Math/AABB.hpp>
#include <vector>

namespace slon {
namespace physics {

// forward
class CollisionObject;

/** Dynamics world or physics scene, as you wish. */
class DynamicsWorld :
    public Referenced
{
public:
    enum COLLISION_TYPE
    {
        CT_DISCRETE,    /// step each frame for dt and check for a collision
        CT_CONTINUOUS   /// capture collision as if they were continous in time
    };

    struct state_desc
#ifdef SLON_ENGINE_USE_SSE
       : public sgl::Aligned16
#endif
    {
        math::AABBf     worldSize;
        COLLISION_TYPE  collisionType;
        math::Vector3f  gravity;
        float           fixedTimeStep;

        state_desc() :
            worldSize( math::Vector3f(0.0f, 0.0f, 0.0f), math::Vector3f(0.0f, 0.0f, 0.0f) ),
            collisionType(CT_DISCRETE),
            gravity( math::Vector3f(0.0f, -9.8f, 0.0f) ),
            fixedTimeStep( 1.0f / 60.0f )
        {}
    };

    typedef std::vector<Contact>            contact_vector;
    typedef contact_vector::iterator        contact_iterator;
    typedef contact_vector::const_iterator  contact_const_iterator;

public:
    /** Setup world's gravity and its direction. */
    virtual void setGravity(const math::Vector3f& gravity) = 0;

    /** Get gravity of our toy world. */
    virtual math::Vector3f getGravity() const = 0;

    /** Get description of the world. */
    virtual const state_desc& getStateDesc() const = 0;

    /** Step simulation. */
    virtual void stepSimulation(float dt) = 0;

    /** Set maximum number of substeps in the simulation step. */
    virtual void setMaxNumSubSteps(unsigned maxSubSteps) = 0;

    /** Get maximum number of substeps in the simulation step. */
    virtual unsigned getMaxNumSubSteps() const = 0;

    /** Get iterator for enumerating active contacts. */
    virtual contact_const_iterator firstActiveContact() const = 0; 

    /** Get iterator end iterator for enumerating active contacts */
    virtual contact_const_iterator endActiveContact() const = 0;

    /** Create rigid body and add it to the world. */
    virtual RigidBody* createRigidBody(const RigidBody::state_desc& rigidBodyDesc) = 0;

    /** Create constraint and add it to the world. */
    virtual Constraint* createConstraint(const Constraint::state_desc& constraintDesc) = 0;

    /** Grant thread read access to the world.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForReading() const = 0;

    /** Grant thread write access to the world.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForWriting() = 0;

    virtual ~DynamicsWorld() {}
};

typedef boost::intrusive_ptr<DynamicsWorld>         dynamics_world_ptr;
typedef boost::intrusive_ptr<const DynamicsWorld>   const_dynamics_world_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__
