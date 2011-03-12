#include "stdafx.h"
#include "Physics/Bullet/BulletConstraint.h"
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

void BulletRotationalServoMotor::reset(BulletConstraint* constraint, int axis)
{
    BulletRotationalMotor::reset(constraint, axis);
    setTargetForce(targetForce);
}

void BulletRotationalServoMotor::solve(real dt)
{
    btVector3 torque = constraint->getBtConstraint()->getAxis(axis) * targetForce;

    btRigidBody& rbA = motor_base::constraint->getBtConstraint()->getRigidBodyA();
    btRigidBody& rbB = motor_base::constraint->getBtConstraint()->getRigidBodyB();

    rbA.applyTorque( torque);
    rbA.activate(true);
    rbB.applyTorque(-torque);
    rbB.activate(true);
}

void BulletRotationalServoMotor::accept(BulletSolverCollector& collector)
{
    if (enableMotor) {
        collector.addSolver(*this);
    }
    BulletRotationalMotor<ServoMotor>::accept(collector);
}

void BulletRotationalServoMotor::setTargetForce(real targetForce_) 
{ 
    targetForce = targetForce_;
    enableMotor = fabs(targetForce) > real(0.01);
}

} // namespace physics
} // namespace slon