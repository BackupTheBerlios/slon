#include "stdafx.h"
#include "Physics/Bullet/BulletRotationalSpringMotor.h"
#include "Physics/Bullet/BulletSolverCollector.h"

namespace slon {
namespace physics {

BulletRotationalSpringMotor::BulletRotationalSpringMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotor<SpringMotor>(constraint, axis)
{
}

void BulletRotationalSpringMotor::solve(real /*dt*/)
{
    real force              = stiffness * (equilibrium - motor->m_currentPosition) - velocityDamping * velocity;
    motor->m_maxMotorForce  = fabs(force);
    motor->m_targetVelocity = force / motor->m_maxMotorForce * btScalar(BT_LARGE_FLOAT);
}

void BulletRotationalSpringMotor::reset(BulletConstraint* constraint, int axis)
{
    bool toggle = motor->m_enableMotor;
    BulletRotationalMotor<SpringMotor>::reset(constraint, axis);
    motor->m_enableMotor = toggle;
}

void BulletRotationalSpringMotor::accept(BulletSolverCollector& collector)
{
    BulletRotationalMotor<SpringMotor>::accept(collector);
    if (motor->m_enableMotor) {
        collector.addSolver(*this);
    }
}

} // namespace physics
} // namespace slon
