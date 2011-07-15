#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletRigidBody.h"
#include "Physics/Bullet/BulletRotationalServoMotor.h"

namespace slon {
namespace physics {

BulletRotationalServoMotor::BulletRotationalServoMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor(constraint, axis)
,   targetForce(0.0f)
,   enableMotor(false)
{
}

void BulletRotationalServoMotor::solve(real dt)
{
    btGeneric6DofConstraint& constraint = BulletRotationalMotor::constraint->getBtConstraint();
   
    btRigidBody& rbA = constraint.getRigidBodyA();
    btRigidBody& rbB = constraint.getRigidBodyB();

    btTransform  trans;
		   /*
    rbA.getMotionState()->getWorldTransform(trans);
    btVector3 rA     = trans.getBasis() * constraint->getFrameOffsetA().getOrigin();
    btVector3 forceA = targetForce * constraint->getAxis(axis).cross(rA) / rA.length2();
    rbA.applyCentralForce(forceA);
    rbA.activate(true);

    rbB.getMotionState()->getWorldTransform(trans);
    btVector3 rB     = trans.getBasis() * constraint->getFrameOffsetB().getOrigin();
    btVector3 forceB = targetForce * constraint->getAxis(axis).cross(rB) / rB.length2();
    rbB.applyCentralForce(forceB);
    rbB.activate(true);
		*/
    btVector3 torque = constraint.getAxis(axis) * targetForce;
    rbA.applyTorque( torque);
    rbA.activate(true);
    rbB.applyTorque(-torque);
    rbB.activate(true);
	/*
    btVector3 torque = constraint->getAxis(axis) * targetForce;
	btScalar  lA     = constraint->getFrameOffsetA().getOrigin().length2();
	btScalar  lB     = constraint->getFrameOffsetB().getOrigin().length2();

    rbA.applyTorque(  torque * lB / (lA + lB) );
    rbA.activate(true);
    rbB.applyTorque( -torque * lA / (lA + lB) );
    rbB.activate(true);
	*/
}

void BulletRotationalServoMotor::setTargetForce(real targetForce_) 
{ 
    targetForce = targetForce_;

    bool enableMotor_ = fabs(targetForce) > real(0.01);
	if (enableMotor_ && !enableMotor) {
		constraint->dynamicsWorld->addSolver(this);
	}
	else if (!enableMotor_ && enableMotor_) {
		constraint->dynamicsWorld->removeSolver(this);
	}
	enableMotor = enableMotor_;
}

} // namespace physics
} // namespace slon
