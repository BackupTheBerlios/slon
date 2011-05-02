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
	assert(constraint_ && axis_ >= 0 && axis_ < 3);
	constraint = constraint_;
    axis       = axis_;
    motor      = constraint->getBtConstraint()->getRotationalLimitMotor(axis);
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
	calculateAngleInfo();
}

template<typename Base>
void BulletRotationalMotor<Base>::calculateAngleInfo()
{
    btGeneric6DofConstraint* bConstraint = constraint->getBtConstraint();

    btRigidBody& rbA = constraint->getBtConstraint()->getRigidBodyA();
    btRigidBody& rbB = constraint->getBtConstraint()->getRigidBodyB();
    btTransform  trans;
    
    // position
    position     = btAdjustAngleToLimits(bConstraint->getAngle(axis), motor->m_loLimit, motor->m_hiLimit);

    // velocity
    btVector3 ax = bConstraint->getAxis(axis);
    velocity     = 0;//ax.dot( rbA.getAngularVelocity() - rbB.getAngularVelocity() ); // angular term

    rbA.getMotionState()->getWorldTransform(trans);
    btVector3 rA = trans.getBasis() * bConstraint->getFrameOffsetA().getOrigin();
    velocity    += rbA.getLinearVelocity().dot( ax.cross(rA) ) / rA.length2(); // linear from A

    rbB.getMotionState()->getWorldTransform(trans);
    btVector3 rB = trans.getBasis() * bConstraint->getFrameOffsetB().getOrigin();
    velocity    += rbB.getLinearVelocity().dot( ax.cross(rB) ) / rB.length2(); // linear from B

    // force
    force = ax.dot( rbA.getTotalTorque() - rbB.getTotalTorque() );
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

template class BulletRotationalMotor<ServoMotor>;
template class BulletRotationalMotor<SpringMotor>;
template class BulletRotationalMotor<VelocityMotor>;

} // namespace physics
} // namespace slon
