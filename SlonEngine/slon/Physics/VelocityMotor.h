#ifndef __SLON_ENGINE_PHYSICS_VELOCITY_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_VELOCITY_MOTOR_H__

#include "Motor.h"

namespace slon {
namespace physics {

/** The motor controlled by velocity. Usually underlying physics engine trying
 * to solve multi-rigid-body simulation problem, keeping the provided velocity constraints,
 * using LCP solver or some similar staff.
 */
class VelocityMotor :
    public Motor
{
private:
	friend class Constraint;
#ifdef SLON_ENGINE_USE_BULLET
	friend class BulletRotationalVelocityMotor;
	typedef BulletRotationalVelocityMotor impl_type;
	typedef boost::scoped_ptr<impl_type>  impl_ptr;
#endif

public:
    // Override Motor
    TYPE              getType() const;
    const Constraint* getConstraint() const;
    Constraint::DOF   getDOF() const;
    math::Vector3r    getAxis() const;
    real              getLoLimit() const;
    real              getHiLimit() const;
    real              getPosition() const;
    real              getVelocity() const;
    real              getForce() const;

    /** Check whether motor is enabled. */
    bool enabled() const;

    /** Enable/Disable motor. */
    void toggle(bool toggle);

    /** Get velocity(angular velocity) which motor is trying to reach. */
    real getTargetVelocity() const;

    /** Set velocity(angular velocity) which motor will try to reach. */
    void setTargetVelocity(real velocity);

    /** Get maximum force(torque) the motor can apply. Must be non negative. */
    real getMaxForce() const;

    /** Set maximum force(torque) the motor can apply. Must be non negative. */
    void setMaxForce(real force);

private:
	VelocityMotor(Constraint* constraint, Constraint::DOF dof);

	// Override Motor
	void instantiate();
	void release();

private:
	Constraint*       constraint;
	Constraint::DOF   dof;
	impl_ptr          impl;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_VELOCITY_DRIVEN_MOTOR_H__
