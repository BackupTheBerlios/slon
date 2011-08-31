#ifndef __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__
#define __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__

#define NOMINMAX
#include "../DynamicsWorld.h"
#include <boost/scoped_ptr.hpp>
#include <sgl/Math/Containers.hpp>

// forward bullet
class btBroadphaseInterface;
class btCollisionConfiguration;
class btCollisionDispatcher;
class btConstraintSolver;
class btDynamicsWorld;

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
class BulletSolver;

/* Bullet implementation of the physics dynamics world. */
class BulletDynamicsWorld :
	public boost::noncopyable
{
friend class BulletRigidBody;
private:
    typedef boost::scoped_ptr<btBroadphaseInterface>    broadphase_ptr;
    typedef boost::scoped_ptr<btCollisionConfiguration> collision_configuration_ptr;
    typedef boost::scoped_ptr<btCollisionDispatcher>    collision_dispatcher_ptr;
    typedef boost::scoped_ptr<btConstraintSolver>       constraint_solver_ptr;
    typedef boost::scoped_ptr<btDynamicsWorld>          dynamics_world_ptr;

	typedef DynamicsWorld::contact_vector               contact_vector;
	typedef DynamicsWorld::contact_const_iterator       contact_const_iterator;
	typedef DynamicsWorld::DESC                   state_desc;
	
private:
    // non copyable
    BulletDynamicsWorld(const BulletDynamicsWorld&);
    BulletDynamicsWorld& operator = (const BulletDynamicsWorld&);

public:
    BulletDynamicsWorld(DynamicsWorld* pInterface);
	~BulletDynamicsWorld();

    // implement dynamics world
    void   setGravity(const math::Vector3r& gravity);
    void   setFixedTimeStep(const real dt)            { /* nothing */ }
    void   setMaxNumSubSteps(unsigned maxSubSteps_)   { /* nothing */ }
    size_t getNumSimulatedSteps() const               { return numSimulatedSteps; }
    real   stepSimulation(real dt);

    contact_const_iterator firstActiveContact() const   { return contacts.begin(); }
    contact_const_iterator endActiveContact() const     { return contacts.end(); }

    // get bullet dynamics world
    btDynamicsWorld& getBtDynamicsWorld() { return *dynamicsWorld; }

	/** Get interface to dynamics world */
	DynamicsWorld* getInterface() { return pInterface; }

	/** Add solver to dynamics world. */
	void addSolver(BulletSolver* solver);

	/** Remove solver from dynamics world. */
	void removeSolver(BulletSolver* solver);

private:
    DynamicsWorld*              pInterface;
    broadphase_ptr              broadPhase;
    collision_configuration_ptr collisionConfiguration;
    collision_dispatcher_ptr    collisionDispatcher;
    constraint_solver_ptr       constraintSolver;
    dynamics_world_ptr          dynamicsWorld;

    // settings 
    size_t                      numSimulatedSteps;
    float                       unsimulatedTime;

	// internal solvers
	BulletSolver*				firstSolver;

    // for handling contact callbacks
    contact_vector              contacts;
    std::vector<math::Vector3r> contactPoints;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_DYNAMICS_WORLD_H__
