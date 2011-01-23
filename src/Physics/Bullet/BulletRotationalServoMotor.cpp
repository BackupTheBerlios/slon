#include "stdafx.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"
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

void BulletRotationalServoMotor::solve(btScalar dt)
{
    math::Vector3f torque = getAxis() * targetForce;
    motor_base::constraint->getRigidBodyA()->applyTorque( torque);
    motor_base::constraint->getRigidBodyB()->applyTorque(-torque);
}

void BulletRotationalServoMotor::accept(BulletSolverCollector& collector)
{
    if (enableMotor) {
        collector.addSolver(*this);
    }
    BulletRotationalMotor<ServoMotor>::accept(collector);
}

void BulletRotationalServoMotor::setTargetForce(float targetForce_) 
{ 
    targetForce             = targetForce_;
    enableMotor             = fabs(targetForce) > 0.01f;
    /*
    motor->m_maxMotorForce  = fabs(targetForce);
    motor->m_enableMotor    = motor->m_maxMotorForce > 0.01f;
    if (motor->m_enableMotor) {
        motor->m_targetVelocity = targetForce / motor->m_maxMotorForce * btScalar(BT_LARGE_FLOAT);
    }*/
}

} // namespace physics
} // namespace slon