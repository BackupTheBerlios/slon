#ifndef __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__

#include "Motor.h"

namespace slon {
namespace physics {

// forward
class BulletRotationalServoMotor;

/** The simpliest motor, can apply force(torque). */
class ServoMotor :
    public Motor
{
private:
	friend class Constraint;
#ifdef SLON_ENGINE_USE_BULLET
	friend class BulletRotationalServoMotor;
	typedef BulletRotationalServoMotor   impl_type;
	typedef boost::scoped_ptr<impl_type> impl_ptr;
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

    /** Check whether motor is enabled (non zero target force). */
    bool enabled() const;

    /** Set force(torque) the motor want to apply. */
    real getTargetForce() const;

    /** Set force(torque) the motor want to apply. */
    void setTargetForce(real force);

private:
	ServoMotor(Constraint* constraint, Constraint::DOF dof);

	/** Create motor implementation, should be called by constraint. */
	void instantiate();

	/** Release motor implementation, should be called by constraint. */
	void release();

private:
	Constraint*       constraint;
	Constraint::DOF   dof;
	impl_ptr          impl;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__
