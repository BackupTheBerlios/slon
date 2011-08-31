#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/PhysicsManager.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletRotationalVelocityMotor.h"

namespace slon {
namespace physics {

BulletRotationalVelocityMotor::BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor(constraint, axis)
{
}

BulletRotationalVelocityMotor::~BulletRotationalVelocityMotor()
{
    motor->m_enableMotor = false;
}

bool BulletRotationalVelocityMotor::enabled() const 
{ 
	return motor->m_enableMotor; 
}

void BulletRotationalVelocityMotor::toggle(bool toggle) 
{ 
    motor->m_enableMotor = toggle; 

    if (motor->m_enableMotor)
    {
        btGeneric6DofConstraint& constraint = BulletRotationalMotor::constraint->getBtConstraint();
        constraint.getRigidBodyA().activate(true);
        constraint.getRigidBodyB().activate(true);
    }
}

real BulletRotationalVelocityMotor::getTargetVelocity() const
{
	return motor->m_targetVelocity;
}

void BulletRotationalVelocityMotor::setTargetVelocity(real velocity) 
{ 
    motor->m_targetVelocity = velocity;

    if ( motor->m_enableMotor && abs(velocity) > physics::real(velocity) )
    {
        btGeneric6DofConstraint& constraint = BulletRotationalMotor::constraint->getBtConstraint();
        constraint.getRigidBodyA().activate(true);
        constraint.getRigidBodyB().activate(true);
    }
}

real BulletRotationalVelocityMotor::getMaxForce() const
{
	return motor->m_maxMotorForce;
}

void BulletRotationalVelocityMotor::setMaxForce(real force) 
{ 
    motor->m_maxMotorForce = force * BulletRotationalMotor::constraint->dynamicsWorld->getInterface()->getDesc().fixedTimeStep; 
}

} // namespace physics
} // namespace slon
