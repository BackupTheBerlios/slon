#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__

#define NOMINMAX
#include "../VelocityMotor.h"
#include "BulletRotationalMotor.h"

namespace slon {
namespace physics {

class BulletRotationalVelocityMotor :
    public BulletRotationalMotor
{
public:
    BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis);
    ~BulletRotationalVelocityMotor();

    // Override VelocityMotor
    bool enabled() const;
    void toggle(bool toggle);

    real getTargetVelocity() const;
    void setTargetVelocity(real velocity);

    real getMaxForce() const;
    void setMaxForce(real force);
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__

