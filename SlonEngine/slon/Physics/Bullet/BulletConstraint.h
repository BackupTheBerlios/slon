#ifndef __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
#define __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__

#include "../Constraint.h"
#include "BulletRotationalMotor.h"

// forward bullet
class btGeneric6DofConstraint;

namespace slon {
namespace physics {

// forward decl
class BulletDynamicsWorld;
class BulletRigidBody;

/** Bullet constraint implementation */
class BulletConstraint :
	public boost::noncopyable
{
friend class BulletDynamicsWorld;
friend class BulletRigidBody;
friend class BulletRotationalMotor;
friend class BulletRotationalServoMotor;
friend class BulletRotationalSpringMotor;
friend class BulletRotationalVelocityMotor;
public:
    typedef boost::scoped_ptr<btGeneric6DofConstraint> bullet_constraint_ptr;

public:
    BulletConstraint(Constraint*              pInterface,
					 BulletDynamicsWorld*     dynamicsWorld,
					 btGeneric6DofConstraint* constraint,
                     const std::string&       name);

    BulletConstraint(Constraint*              pInterface,
					 BulletDynamicsWorld*     dynamicsWorld);

	~BulletConstraint();

    /** Get internal bullet constraint */
    btGeneric6DofConstraint& getBtConstraint() { return *constraint; }

    /** Get internal bullet constraint */
    const btGeneric6DofConstraint& getBtConstraint() const { return *constraint; }

    // Override Constraint
    math::Vector3r   getAxis(unsigned int axis) const;
    real             getPosition(Constraint::DOF dof) const;

private:
    Constraint*             pInterface;
    BulletDynamicsWorld*    dynamicsWorld;
    bullet_constraint_ptr   constraint;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
