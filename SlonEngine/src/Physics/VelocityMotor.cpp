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
    if (impl) {
        return impl->getLoLimit();
    }
    else if (dof > 3) {
        return constraint->getStateDesc().angularLimits[0][dof - 3];
    }
    else {
        return constraint->getStateDesc().linearLimits[0][dof];
    }
}

real VelocityMotor::getHiLimit() const
{
    if (impl) {
        return impl->getHiLimit();
    }
    else if (dof > 3) {
        return constraint->getStateDesc().angularLimits[1][dof - 3];
    }
    else {
        return constraint->getStateDesc().linearLimits[1][dof];
    }
}

real VelocityMotor::getPosition() const
{
    return impl ?impl->getPosition() : 0;
}

real VelocityMotor::getVelocity() const
{
    return impl ? impl->getVelocity() : 0;
}

real VelocityMotor::getForce() const
{
    return impl ? getForce() : 0;
}

bool VelocityMotor::enabled() const
{
    return impl ? impl->enabled() : false;
}

void VelocityMotor::toggle(bool toggle)
{
    assert(impl);
	impl->toggle(toggle);
}

real VelocityMotor::getTargetVelocity() const
{
    assert(impl);
	return impl->getTargetVelocity();
}

void VelocityMotor::setTargetVelocity(real force)
{
    assert(impl);
	impl->setTargetVelocity(force);
}

real VelocityMotor::getMaxForce() const
{
    assert(impl);
	return impl->getMaxForce();
}

void VelocityMotor::setMaxForce(real force)
{
    assert(impl);
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
