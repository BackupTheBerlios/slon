#include "stdafx.h"
#include "Physics/ServoMotor.h"
#include "Physics/SpringMotor.h"
#include "Physics/VelocityMotor.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRotationalMotor.h"
#include "Physics/Bullet/BulletSolverCollector.h"

namespace slon {
namespace physics {
    
BulletRotationalMotorBase::BulletRotationalMotorBase(BulletConstraint* constraint_,
                                                     int               axis_)
:   constraint(constraint_)
,   axis(axis_)
{
    assert(constraint && axis >= 0 && axis < 3);
    motor = constraint->getBtConstraint()->getRotationalLimitMotor(axis);
}

void BulletRotationalMotorBase::reset(BulletConstraint* constraint_, int axis_)
{
    constraint = constraint_;
    axis       = axis_;
    motor      = constraint->getBtConstraint()->getRotationalLimitMotor(axis);
    assert(constraint && axis >= 0 && axis < 3);
}

template<typename Base>
BulletRotationalMotor<Base>::BulletRotationalMotor(BulletConstraint* constraint, int axis)
:   BulletRotationalMotorBase(constraint, axis)
{
    calculateAngleInfo();
}

template<typename Base>
void BulletRotationalMotor<Base>::reset(BulletConstraint* constraint, int axis)
{
    BulletRotationalMotorBase::reset(constraint, axis);
    velocity = 0.0f;
    force    = 0.0f;
}

template<typename Base>
void BulletRotationalMotor<Base>::calculateAngleInfo()
{
    btRigidBody& bodyA = constraint->getBtConstraint()->getRigidBodyA();
    btRigidBody& bodyB = constraint->getBtConstraint()->getRigidBodyB();

    // position
    position     = btAdjustAngleToLimits(constraint->getBtConstraint()->getAngle(axis),motor->m_loLimit, motor->m_hiLimit);

    // velocity
    btVector3 ax = constraint->getBtConstraint()->getAxis(axis);
    btVector3 a  = bodyA.getAngularVelocity();
    btVector3 b  = bodyB.getAngularVelocity();
    velocity     = ax.dot(a - b);

    // force
    a     = bodyA.getTotalTorque();
    b     = bodyB.getTotalTorque();
    force = ax.dot(a - b);
}

template<typename Base>
void BulletRotationalMotor<Base>::accept(BulletSolverCollector& collector)
{
    calculateAngleInfo();
}

template<typename Base>
const Constraint* BulletRotationalMotor<Base>::getConstraint() const 
{ 
    return constraint; 
}

template<typename Base>
math::Vector3r BulletRotationalMotor<Base>::getAxis() const       
{ 
    return to_vec(constraint->getBtConstraint()->getAxis(axis)); 
}

template BulletRotationalMotor<ServoMotor>;
template BulletRotationalMotor<SpringMotor>;
template BulletRotationalMotor<VelocityMotor>;

} // namespace physics
} // namespace slon