#ifndef __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__
#define __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__

#include "../Utility/referenced.hpp"
#include "CollisionObject.h"
#include <sgl/Math/AABB.hpp>
#include <vector>

namespace slon {
namespace physics {

#ifdef SLON_ENGINE_USE_BULLET
    class BulletDynamicsWorld;
#endif

/** Dynamics world or physics scene, as you wish. */
class SLON_PUBLIC DynamicsWorld :
    public Referenced
{
private:
#ifdef SLON_ENGINE_USE_BULLET
    friend class BulletDynamicsWorld;
	typedef BulletDynamicsWorld          impl_type;
    typedef boost::scoped_ptr<impl_type> impl_ptr;
#endif

public:
    enum COLLISION_TYPE
    {
        CT_DISCRETE,    /// step each frame for dt and check for a collision
        CT_CONTINUOUS   /// capture collision as if they were continous in time
    };

    struct SLON_PUBLIC DESC
#ifdef SLON_ENGINE_USE_SSE
       : public aligned<0x10>
#endif
    {
        math::AABBr     worldSize;
        COLLISION_TYPE  collisionType;
        math::Vector3r  gravity;
        real            fixedTimeStep;
        size_t          maxNumSubSteps;

        DESC() :
            worldSize( math::Vector3r(0), math::Vector3r(0) ),
            collisionType(CT_DISCRETE),
            gravity(0, real(-9.8), 0),
            fixedTimeStep( real(1.0 / 60.0) ),
            maxNumSubSteps(3)
        {}
    };

    typedef std::vector<Contact>            contact_vector;
    typedef contact_vector::iterator        contact_iterator;
    typedef contact_vector::const_iterator  contact_const_iterator;

public:
    DynamicsWorld(const DESC& desc);
    ~DynamicsWorld();

    /** Setup world's gravity and its direction. */
    void setGravity(const math::Vector3r& gravity);

    /** Get gravity of our toy world. */
    math::Vector3r getGravity() const;

    /** Set fixed time step for simulation */
    void setFixedTimeStep(const real dt);

    /** Set fixed time step of the simulation */
    real getFixedTimeStep() const;

    /** Get number of simulation steps performed */
    size_t getNumSimulatedSteps() const;

    /** Get description of the world. */
    const DESC& getDesc() const;

    /** Step simulation. 
     * @return "unsimulated" time < fixedTimeStep
     */
    real stepSimulation(real dt);

    /** Set maximum number of substeps in the simulation step. */
    void setMaxNumSubSteps(unsigned maxSubSteps);

    /** Get maximum number of substeps in the simulation step. */
    unsigned getMaxNumSubSteps() const;

    /** Get iterator for enumerating active contacts. */
    contact_const_iterator firstActiveContact() const; 

    /** Get iterator end iterator for enumerating active contacts */
    contact_const_iterator endActiveContact() const;

    /** Add rigid body into the world. */
    void addRigidBody(RigidBody* rigidBody);

    /** Add constraint into the world. */
    void addConstraint(Constraint* constraint);

    /** Remove rigid body from the world. Return true if succeeded. */
    bool removeRigidBody(RigidBody* rigidBody);

    /** Add constraint into the world. Return true if succeeded. */
    bool removeConstraint(Constraint* constraint);

	/** Get implementation object */
	impl_type* getImpl() { return impl.get(); }

	/** Get implementation object */
	const impl_type* getImpl() const { return impl.get(); }

private:
	DESC       desc;
	impl_ptr   impl;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_DYNAMICS_WORLD_H__
