#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletRotationalMotor.h"

namespace slon {
namespace physics {
    
BulletRotationalMotor::BulletRotationalMotor(BulletConstraint* constraint_,
                                             int               axis_)
:   constraint(constraint_)
,   axis(axis_)
{
    assert(constraint && axis >= 0 && axis < 3);
    motor = constraint->getBtConstraint().getRotationalLimitMotor(axis);
}

void BulletRotationalMotor::calculateAngleInfo() const
{
	// check if we have to update info
	size_t numWorldSimulatedSteps = constraint->dynamicsWorld->getNumSimulatedSteps();
	if (numSimulatedSteps < numWorldSimulatedSteps) 
	{
		btGeneric6DofConstraint& bConstraint = constraint->getBtConstraint();

		btRigidBody& rbA = bConstraint.getRigidBodyA();
		btRigidBody& rbB = bConstraint.getRigidBodyB();
		btTransform  trans;
    
		// get offset from rigid body CM to joint
		rbA.getMotionState()->getWorldTransform(trans);
		btVector3 rA = trans.getBasis() * bConstraint.getFrameOffsetA().getOrigin();

		rbB.getMotionState()->getWorldTransform(trans);
		btVector3 rB = trans.getBasis() * bConstraint.getFrameOffsetB().getOrigin();
    
		// position
		position = btAdjustAngleToLimits(bConstraint.getAngle(axis), motor->m_loLimit, motor->m_hiLimit);

		// convert angular to linear
		btVector3 ax   = bConstraint.getAxis(axis);
		btVector3 velA(0,0,0);// = rA.cross( rbA.getAngularVelocity() );
		btVector3 velB(0,0,0);// = rB.cross( rbB.getAngularVelocity() );

		// correct linear to angular
		velocity  = (velA + rbA.getLinearVelocity()).dot( ax.cross(rA) ) / rA.length2();
		velocity += (velB + rbB.getLinearVelocity()).dot( ax.cross(rB) ) / rB.length2();

		// force
		force = ax.dot( rbA.getTotalTorque() - rbB.getTotalTorque() );

		numSimulatedSteps = numWorldSimulatedSteps;
	}

}

math::Vector3r BulletRotationalMotor::getAxis() const       
{ 
    return to_vec(constraint->getBtConstraint().getAxis(axis)); 
}

real BulletRotationalMotor::getLoLimit() const
{
	return motor->m_loLimit; 
}

real BulletRotationalMotor::getHiLimit() const
{ 
	return motor->m_hiLimit; 
}

real BulletRotationalMotor::getPosition() const
{ 
	calculateAngleInfo();
	return position; 
}

real BulletRotationalMotor::getVelocity() const
{ 
	calculateAngleInfo();
	return velocity;
}

real BulletRotationalMotor::getForce() const
{ 
	calculateAngleInfo();
	return force;
}

} // namespace physics
} // namespace slon
