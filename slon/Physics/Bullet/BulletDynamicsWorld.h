#ifndef __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__
#define __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__

#define NOMINMAX
#include "../DynamicsWorld.h"
#include "BulletConstraint.h"
#include "BulletSolverCollector.h"
#include <boost/intrusive/slist.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <sgl/Math/Containers.hpp>

namespace slon {
namespace physics {

// predicate for sorting contacts
struct compare_contact
{
    bool operator() (const Contact& a, const Contact& b)
    {
        if (a.collisionObjects[0] < b.collisionObjects[0]) {
            return true;
        }
        
        if (a.collisionObjects[0] > b.collisionObjects[0]) {
            return false;
        }
        
        return a.collisionObjects[1] < b.collisionObjects[1];
    }
};

class BulletConstraint;

/* Bullet implementation of the physics dynamics world. */
class BulletDynamicsWorld :
    public DynamicsWorld
{
friend class BulletRigidBody;
private:
    typedef boost::shared_ptr<btBroadphaseInterface>    broadphase_ptr;
    typedef boost::shared_ptr<btCollisionConfiguration> collision_configuration_ptr;
    typedef boost::shared_ptr<btCollisionDispatcher>    collision_dispatcher_ptr;
    typedef boost::shared_ptr<btConstraintSolver>       constraint_solver_ptr;
    typedef boost::shared_ptr<btDynamicsWorld>          dynamics_world_ptr;

    typedef boost::intrusive::member_hook
    < 
        BulletConstraint, 
        BulletConstraint::slist_hook, 
        &BulletConstraint::dynamicsWorldHook 
    > constraint_hook;

    typedef boost::intrusive::slist
    <
        BulletConstraint, 
        constraint_hook,
        boost::intrusive::constant_time_size<false> 
    > constraint_list;

private:
    // non copyable
    BulletDynamicsWorld(const BulletDynamicsWorld&);
    BulletDynamicsWorld& operator = (const BulletDynamicsWorld&);

public:
    BulletDynamicsWorld(const state_desc& desc);

    //
    void                accept(BulletSolverCollector& collector);

    // Override dynamics world
    void                setGravity(const math::Vector3r& gravity);
    math::Vector3r      getGravity() const;
    const state_desc&   getStateDesc() const;
    void                stepSimulation(real dt);
    void                setMaxNumSubSteps(unsigned maxSubSteps_)    { maxSubSteps = maxSubSteps_; }
    unsigned            getMaxNumSubSteps() const                   { return maxSubSteps; }

    RigidBody*          createRigidBody(const RigidBody::state_desc& rigidBodyDesc);
    Constraint*         createConstraint(const Constraint::state_desc& constraintDesc);

    /** Get maximum number of substeps in the simulation step. */
    contact_const_iterator firstActiveContact() const   { return contacts.begin(); }
    contact_const_iterator endActiveContact() const     { return contacts.end(); }

    thread::lock_ptr    lockForReading() const;
    thread::lock_ptr    lockForWriting();

    // get bullet dynamics world
    btDynamicsWorld& getBtDynamicsWorld() { return *dynamicsWorld; }

private:
    state_desc                  desc;
    broadphase_ptr              broadPhase;
    collision_configuration_ptr collisionConfiguration;
    collision_dispatcher_ptr    collisionDispatcher;
    constraint_solver_ptr       constraintSolver;
    dynamics_world_ptr          dynamicsWorld;

    // items
    constraint_list             constraints;

    // proprietary solvers
    BulletSolverCollector       solverCollector;

    // settings 
    unsigned                    maxSubSteps;

    // for handling contact callbacks
    contact_vector              contacts;
    std::vector<math::Vector3r> contactPoints;

    // mutex locks any camera modification
    mutable boost::shared_mutex accessMutex;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__
