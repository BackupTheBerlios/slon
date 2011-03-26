#include "stdafx.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRotationalVelocityMotor.h"
#include "Physics/Bullet/BulletSolverCollector.h"

namespace slon {
namespace physics {

BulletRotationalVelocityMotor::BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor<VelocityMotor>(constraint, axis)
{
}

BulletRotationalVelocityMotor::~BulletRotationalVelocityMotor()
{
    motor->m_enableMotor = false;
}

void BulletRotationalVelocityMotor::reset(BulletConstraint* constraint_, int axis_)
{
    btRotationalLimitMotor* motor_ = constraint_->getBtConstraint()->getRotationalLimitMotor(axis_);
    motor_->m_enableMotor          = motor->m_enableMotor;
    motor_->m_maxMotorForce        = motor->m_maxMotorForce;
    motor_->m_targetVelocity       = motor->m_targetVelocity;
    BulletRotationalMotor<VelocityMotor>::reset(constraint_, axis_);
}

void BulletRotationalVelocityMotor::toggle(bool toggle) 
{ 
    motor->m_enableMotor = toggle; 

    if (motor->m_enableMotor)
    {
        btGeneric6DofConstraint* constraint = motor_base::constraint->getBtConstraint();
        constraint->getRigidBodyA().activate(true);
        constraint->getRigidBodyB().activate(true);
    }
}

void BulletRotationalVelocityMotor::setTargetVelocity(real velocity) 
{ 
    motor->m_targetVelocity = velocity;

    if ( motor->m_enableMotor && abs(velocity) > physics::real(velocity) )
    {
        btGeneric6DofConstraint* constraint = motor_base::constraint->getBtConstraint();
        constraint->getRigidBodyA().activate(true);
        constraint->getRigidBodyB().activate(true);
    }
}

void BulletRotationalVelocityMotor::setMaxForce(real force) 
{ 
    motor->m_maxMotorForce = force * currentPhysicsManager().getDynamicsWorld()->getStateDesc().fixedTimeStep; 
}

} // namespace physics
} // namespace slon
