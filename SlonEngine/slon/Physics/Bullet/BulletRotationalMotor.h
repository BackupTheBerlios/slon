#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__

#include "../Motor.h"

// forward bullet
class btRotationalLimitMotor;

namespace slon {
namespace physics {

// forward
class BulletConstraint;

class BulletRotationalMotor
{
public:
    BulletRotationalMotor(Constraint* constraint, int axis);

    // Implement Motor
    math::Vector3r     getAxis() const;
    real               getLoLimit() const;
    real               getHiLimit() const;
    real               getPosition() const;
    real               getVelocity() const;
    real               getForce() const;

    /** Get internal bullet motor */
    virtual btRotationalLimitMotor& getBtMotor() { return *motor; }

    /** Get internal bullet motor */
    virtual const btRotationalLimitMotor& getBtMotor() const { return *motor; }

protected:
    void calculateAngleInfo() const;

protected:
    BulletConstraint*       constraint;
    int                     axis;
    btRotationalLimitMotor* motor;
    mutable size_t          numSimulatedSteps;
    mutable real            position;
    mutable real            velocity;
    mutable real            force;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__