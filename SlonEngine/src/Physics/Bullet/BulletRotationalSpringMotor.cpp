#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletRotationalSpringMotor.h"

namespace slon {
namespace physics {

BulletRotationalSpringMotor::BulletRotationalSpringMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor(constraint, axis)
,	enableMotor(0)
,	maxForce(1)
,	equilibrium(0)
,	stiffness(1)
,	velocityDamping(0.1)
{
}

void BulletRotationalSpringMotor::toggle(bool enableMotor_)
{
	if (enableMotor_ && !enableMotor) {
		constraint->dynamicsWorld->addSolver(this);
	}
	else if (!enableMotor_ && enableMotor_) {
		constraint->dynamicsWorld->removeSolver(this);
	}
	enableMotor = enableMotor_;
}

void BulletRotationalSpringMotor::solve(real /*dt*/)
{
    btScalar force = stiffness * (equilibrium - getPosition()) - velocityDamping * getVelocity();
	if (force > maxForce) {
		force = maxForce;
	}
	else if (force < -maxForce) {
		force = -maxForce;
	}
	
    btGeneric6DofConstraint& constraint = BulletRotationalMotor::constraint->getBtConstraint();
    btRigidBody& rbA = constraint.getRigidBodyA();
    btRigidBody& rbB = constraint.getRigidBodyB();
    btVector3 torque = constraint.getAxis(axis) * force;

    rbA.applyTorque( torque);
    rbA.activate(true);
    rbB.applyTorque(-torque);
    rbB.activate(true);
}

} // namespace physics
} // namespace slon
