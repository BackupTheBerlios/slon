#ifndef __SLON_ENGINE_PHYSICS_SPRING_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_SPRING_MOTOR_H__

#include "Motor.h"

namespace slon {
namespace physics {

/** The motor that mimics spring. You can control spring equilibrium point,
 * spring stiffness, and velocity damping. If you are familiar with the
 * controllers this motor is very similar to PD controller. The force formula
 * can differ for different implementation, but should mimiv this one: F = stiffness * (equilibrium - position) - damping * velocity.
 */
class SpringMotor :
    public Motor
{
public:
    /** Check wether motor is enabled. */
    virtual bool enabled() const = 0;

    /** Enable/Disable motor. */
    virtual void toggle(bool toggle) = 0;

    /** Get velocity damping */
    virtual float getVelocityDamping() const = 0;

    /** Get velocity damping. */
    virtual void setVelocityDamping(float damping) = 0;

    /** Get spring stiffness. */
    virtual float getStiffness() const = 0;

    /** Set spring stiffness. */
    virtual void setStiffness(float stiffness) = 0;
    
    /** Get equilibrium point of the spring. */
    virtual float getEquilibriumPoint() const = 0;

    /** Set equilibrium point for the spring. */
    virtual void setEquilibriumPoint(float equilibrium) = 0;

    /** Get maximum force(torque) the motor can apply. Must be non negative. */
    virtual float getMaxForce() const = 0;

    /** Set maximum force(torque) the motor can apply. Must be non negative. */
    virtual void setMaxForce(float force) = 0;

    virtual ~SpringMotor() {} 
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_SPRING_MOTOR_H__
