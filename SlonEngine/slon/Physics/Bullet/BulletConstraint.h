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
class BulletConstraint
{
friend class BulletDynamicsWorld;
friend class BulletRigidBody;
friend class BulletRotationalMotor;
friend class BulletRotationalServoMotor;
friend class BulletRotationalVelocityMotor;
public:
    typedef boost::scoped_ptr<btGeneric6DofConstraint> bullet_constraint_ptr;

private:
    // non copyable
    BulletConstraint(const BulletConstraint&);
    BulletConstraint& operator = (const BulletConstraint&);

    // Destroy constraint
    void destroy();

public:
    BulletConstraint(Constraint*              pInterface,
					 DynamicsWorld*           dynamicsWorld,
					 btGeneric6DofConstraint* constraint,
                     const std::string&       name);

    BulletConstraint(Constraint*      pInterface,
					 DynamicsWorld*   dynamicsWorld);

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
