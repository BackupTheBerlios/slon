#ifndef __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
#define __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__

#include "../Constraint.h"
#include "BulletRotationalMotor.h"
#include <boost/intrusive/slist.hpp>

namespace slon {
namespace physics {

// forwad decl
class BulletDynamicsWorld;
class BulletRigidBody;

/** Bullet constraint implementation */
class BulletConstraint :
    public Constraint
{
friend class BulletDynamicsWorld;
friend class BulletRigidBody;
public:
    typedef boost::scoped_ptr<btGeneric6DofConstraint>   bullet_constraint_ptr;
    typedef boost::intrusive_ptr<BulletDynamicsWorld>    dynamics_world_ptr;
    typedef boost::scoped_ptr<BulletRotationalMotorBase> rotational_motor_ptr;

private:
    typedef boost::intrusive::slist_member_hook
    < 
        boost::intrusive::link_mode<boost::intrusive::auto_unlink> 
    > slist_hook;

private:
    // non copyable
    BulletConstraint(const BulletConstraint&);
    BulletConstraint& operator = (const BulletConstraint&);

    // Unlink constraint from rigid body
    void unlink(const RigidBody& rigidBody);

    // Destroy constraint
    void destroy();

public:
    BulletConstraint(btGeneric6DofConstraint* constraint,
                     const std::string&       name);
    BulletConstraint(Constraint*      pInterface,
					 DynamicsWorld*   dynamicsWorld);
	~BulletConstraint();

    /** Get internal bullet constraint */
    btGeneric6DofConstraint* getBtConstraint() { return constraint.get(); }

    /** Get internal bullet constraint */
    const btGeneric6DofConstraint* getBtConstraint() const { return constraint.get(); }

    // Override BulletObject
    void accept(BulletSolverCollector& collector);

    // Override Constraint
    RigidBody*       getRigidBodyA() const { return desc.rigidBodies[0].get(); }
    RigidBody*       getRigidBodyB() const { return desc.rigidBodies[1].get(); }

    math::Vector3r   getAxis(unsigned int axis) const;
    real             getPosition(Constraint::DOF dof) const;

private:
    Constraint*             pInterface;
    dynamics_world_ptr      dynamicsWorld;
    bullet_constraint_ptr   constraint;
    slist_hook              dynamicsWorldHook; // to store in dynamics World
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
