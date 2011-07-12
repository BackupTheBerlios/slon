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
public:
    /** Check wether motor is enabled. */
    virtual bool enabled() const = 0;

    /** Enable/Disable motor. */
    virtual void toggle(bool toggle) = 0;

    /** Get velocity(angular velocity) which motor is trying to reach. */
    virtual real getTargetVelocity() const = 0;

    /** Set velocity(angular velocity) which motor will try to reach. */
    virtual void setTargetVelocity(real velocity) = 0;

    /** Get maximum force(torque) the motor can apply. Must be non negative. */
    virtual real getMaxForce() const = 0;

    /** Set maximum force(torque) the motor can apply. Must be non negative. */
    virtual void setMaxForce(real force) = 0;

    virtual ~VelocityMotor() {} 
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_VELOCITY_DRIVEN_MOTOR_H__
