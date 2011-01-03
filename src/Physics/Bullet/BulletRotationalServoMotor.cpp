#include "stdafx.h"
#include "Physics/Bullet/BulletRotationalServoMotor.h"

namespace slon {
namespace physics {

BulletRotationalServoMotor::BulletRotationalServoMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor(constraint, axis)
,   targetForce(0.0f)
{
}

void BulletRotationalServoMotor::reset(BulletConstraint* constraint, int axis)
{
    BulletRotationalMotor::reset(constraint, axis);
    setTargetForce(targetForce);
}

void BulletRotationalServoMotor::setTargetForce(float targetForce_) 
{ 
    targetForce             = targetForce_;
    motor->m_maxMotorForce  = fabs(targetForce);
    motor->m_enableMotor    = motor->m_maxMotorForce > 0.01f;
    if (motor->m_enableMotor) {
        motor->m_targetVelocity = targetForce / motor->m_maxMotorForce * btScalar(BT_LARGE_FLOAT);
    }
}

} // namespace physics
} // namespace slon