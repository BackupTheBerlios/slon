#ifndef __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__
#define __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__

#include <sgl/Math/AABB.hpp>
#include <vector>
#include "../Thread/Lock.h"
#include "../Utility/referenced.hpp"
#include "PhysicsModel.h"

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
       : public aligned<0x10>
#endif
    {
        math::AABBr     worldSize;
        COLLISION_TYPE  collisionType;
        math::Vector3r  gravity;
        real            fixedTimeStep;

        state_desc() :
            worldSize( math::Vector3r(0), math::Vector3r(0) ),
            collisionType(CT_DISCRETE),
            gravity(0, real(-9.8), 0),
            fixedTimeStep( real(1.0 / 60.0) )
        {}
    };

    typedef std::vector<Contact>            contact_vector;
    typedef contact_vector::iterator        contact_iterator;
    typedef contact_vector::const_iterator  contact_const_iterator;

public:
    /** Setup world's gravity and its direction. */
    virtual void setGravity(const math::Vector3r& gravity) = 0;

    /** Get gravity of our toy world. */
    virtual math::Vector3r getGravity() const = 0;

    /** Set fixed time step for simulation */
    virtual void setFixedTimeStep(const real dt) = 0;

    /** Set fixed time step of the simulation */
    virtual real getFixedTimeStep() const = 0;

	/** Get number of simulation steps performed */
	virtual size_t getNumSimulatedSteps() const = 0;

    /** Get description of the world. */
    virtual const state_desc& getStateDesc() const = 0;

    /** Step simulation. 
     * @return "unsimulated" time < fixedTimeStep
     */
    virtual real stepSimulation(real dt) = 0;

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
