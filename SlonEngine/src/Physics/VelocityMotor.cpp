#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Physics/VelocityMotor.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletRotationalVelocityMotor.h"
#endif

namespace slon {
namespace physics {

VelocityMotor::VelocityMotor(Constraint*     constraint_, 
	                         Constraint::DOF dof_)
:	constraint(constraint_)
,	dof(dof_)
{
}

Motor::TYPE VelocityMotor::getType() const
{
	return MOTOR_SERVO;
}

const Constraint* VelocityMotor::getConstraint() const
{
	return constraint;
}

Constraint::DOF VelocityMotor::getDOF() const
{
	return dof;
}

math::Vector3r VelocityMotor::getAxis() const
{
	return impl->getAxis();
}

real VelocityMotor::getLoLimit() const
{
	return impl->getLoLimit();
}

real VelocityMotor::getHiLimit() const
{
	return impl->getHiLimit();
}

real VelocityMotor::getPosition() const
{
	return impl->getPosition();
}

real VelocityMotor::getVelocity() const
{
	return impl->getVelocity();
}

real VelocityMotor::getForce() const
{
	return impl->getForce();
}

bool VelocityMotor::enabled() const
{
	return impl->enabled();
}

void VelocityMotor::toggle(bool toggle)
{
	impl->toggle(toggle);
}

real VelocityMotor::getTargetVelocity() const
{
	return impl->getTargetVelocity();
}

void VelocityMotor::setTargetVelocity(real force)
{
	impl->setTargetVelocity(force);
}

real VelocityMotor::getMaxForce() const
{
	return impl->getMaxForce();
}

void VelocityMotor::setMaxForce(real force)
{
	impl->setMaxForce(force);
}

void VelocityMotor::instantiate()
{
	impl.reset( new impl_type(constraint->getImpl(), dof - 3) );
}

void VelocityMotor::release()
{
	impl.reset();
}

} // namespace physics
} // namespace slon
