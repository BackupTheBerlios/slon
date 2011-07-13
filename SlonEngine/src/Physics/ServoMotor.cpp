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
	return impl->getAxis();
}

real ServoMotor::getLoLimit() const
{
	return impl->getLoLimit();
}

real ServoMotor::getHiLimit() const
{
	return impl->getHiLimit();
}

real ServoMotor::getPosition() const
{
	return impl->getPosition();
}

real ServoMotor::getVelocity() const
{
	return impl->getVelocity();
}

real ServoMotor::getForce() const
{
	return impl->getForce();
}

bool ServoMotor::enabled() const
{
	return impl->enabled();
}

real ServoMotor::getTargetForce() const
{
	return impl->getTargetForce();
}

void ServoMotor::setTargetForce(real force)
{
	impl->setTargetForce(force);
}

void ServoMotor::instantiate()
{
	impl.reset( new impl_type(constraint->getImpl(), dof) );
}

void ServoMotor::release()
{
	impl.reset();
}

} // namespace physics
} // namespace slon
