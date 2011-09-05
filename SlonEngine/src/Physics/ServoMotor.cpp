#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Physics/ServoMotor.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletRotationalServoMotor.h"
#endif

namespace slon {
namespace physics {

ServoMotor::ServoMotor(Constraint*     constraint_, 
	                   Constraint::DOF dof_)
:	constraint(constraint_)
,	dof(dof_)
{
}

ServoMotor::~ServoMotor()
{
}

Motor::TYPE ServoMotor::getType() const
{
	return MOTOR_SERVO;
}

const Constraint* ServoMotor::getConstraint() const
{
	return constraint;
}

Constraint::DOF ServoMotor::getDOF() const
{
	return dof;
}

math::Vector3r ServoMotor::getAxis() const
{
    return constraint->getAxis(dof >= 3 ? dof - 3 : dof);
}

real ServoMotor::getLoLimit() const
{
    if (impl) {
        return impl->getLoLimit();
    }
    else if (dof > 3) {
        return constraint->getDesc().angularLimits[0][dof - 3];
    }
    else {
        return constraint->getDesc().linearLimits[0][dof];
    }
}

real ServoMotor::getHiLimit() const
{
    if (impl) {
        return impl->getHiLimit();
    }
    else if (dof > 3) {
        return constraint->getDesc().angularLimits[1][dof - 3];
    }
    else {
        return constraint->getDesc().linearLimits[1][dof];
    }
}

real ServoMotor::getPosition() const
{
    return impl ? impl->getPosition() : 0;
}

real ServoMotor::getVelocity() const
{
    return impl ? impl->getVelocity() : 0;
}

real ServoMotor::getForce() const
{
    return impl ? impl->getForce() : 0;
}

bool ServoMotor::enabled() const
{
    return (abs(getForce()) > 0);
}

real ServoMotor::getTargetForce() const
{
    assert(impl);
	return impl->getTargetForce();
}

void ServoMotor::setTargetForce(real force)
{
    assert(impl);
	impl->setTargetForce(force);
}

void ServoMotor::instantiate()
{
	impl.reset( new impl_type(constraint->getImpl(), dof - 3) );
}

void ServoMotor::release()
{
	impl.reset();
}

} // namespace physics
} // namespace slon
