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
    btGeneric6DofConstraint* constraint = motor_base::constraint->getBtConstraint();
   
    btRigidBody& rbA = constraint->getRigidBodyA();
    btRigidBody& rbB = constraint->getRigidBodyB();

    btVector3 torque = constraint->getAxis(axis) * targetForce;
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