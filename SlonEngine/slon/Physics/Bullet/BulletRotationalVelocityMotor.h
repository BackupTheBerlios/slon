#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__

#define NOMINMAX
#include "../VelocityMotor.h"
#include "BulletRotationalMotor.h"

namespace slon {
namespace physics {

class BulletRotationalVelocityMotor :
    public BulletRotationalMotor<VelocityMotor>
{
typedef BulletRotationalMotor<VelocityMotor> motor_base;
public:
    BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis);
    ~BulletRotationalVelocityMotor();

    // Override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);

    // Override VelocityMotor
    bool enabled() const { return motor->m_enableMotor; }
    void toggle(bool toggle);

    real getTargetVelocity() const { return motor->m_targetVelocity; }
    void setTargetVelocity(real velocity);

    real getMaxForce() const { return motor->m_maxMotorForce; }
    void setMaxForce(real force);
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__
