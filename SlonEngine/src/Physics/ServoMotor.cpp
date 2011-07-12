#ifndef __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__

#include "Motor.h"

namespace slon {
namespace physics {

/** The simpliest motor, can apply force(torque). */
class ServoMotor :
    public Motor
{
public:
    /** Check wether motor is enabled (non zero target force) */
    virtual bool enabled() const = 0;

    /** Set force(torque) the motor want to apply. */
    virtual real getTargetForce() const = 0;

    /** Set force(torque) the motor want to apply. */
    virtual void setTargetForce(real force) = 0;

    virtual ~ServoMotor() {} 
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_SERVO_MOTOR_H__
