#ifndef __SLON_ENGINE_PHYSICS_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_MOTOR_H__

#include "Forward.h"
#include <sgl/Math/Vector.hpp>

namespace slon {
namespace physics {

class Constraint;

/** Interface for rotational or translational motor. For rotational motor force means torque. */
class Motor
{
public:
    enum TYPE
    {
        MOTOR_X_TRANS,
        MOTOR_Y_TRANS,
        MOTOR_Z_TRANS,
        MOTOR_X_ROT,
        MOTOR_Y_ROT,
        MOTOR_Z_ROT
    };

public:
    /** Get constraint to which motor is attached */
    virtual const Constraint* getConstraint() const = 0;

    /** Get motor type */
    virtual TYPE getType() const = 0;

    /** Get working axis of the motor. For rotational motor it axis along which the rotation is performed,
     * for translational motor it is axis where motor is translating.
     */
    virtual math::Vector3r getAxis() const = 0;

    /** Get lower limit of the motor. */
    virtual real getLoLimit() const = 0;

    /** Get high limit of the motor. */
    virtual real getHiLimit() const = 0;

    /** Get position(rotation) of the motor. */
    virtual real getPosition() const = 0;

    /** Get velocity(angular velocity) of the motor. */
    virtual real getVelocity() const = 0;

    /** Get applied motor force(torque) */
    virtual real getForce() const = 0;

    virtual ~Motor() {} 
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_MOTOR_H__
