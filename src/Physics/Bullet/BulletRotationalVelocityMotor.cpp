#include "stdafx.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRotationalVelocityMotor.h"
#include "Physics/Bullet/BulletSolverCollector.h"

namespace slon {
namespace physics {

BulletRotationalVelocityMotor::BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor(constraint, axis)
{
}

void BulletRotationalVelocityMotor::reset(BulletConstraint* constraint_, int axis_)
{
    btRotationalLimitMotor* motor_ = constraint_->getBtConstraint()->getRotationalLimitMotor(axis_);
    motor_->m_enableMotor          = motor->m_enableMotor;
    motor_->m_maxMotorForce        = motor->m_maxMotorForce;
    motor_->m_targetVelocity       = motor->m_targetVelocity;
    BulletRotationalMotor::reset(constraint_, axis_);
}

} // namespace physics
} // namespace slon