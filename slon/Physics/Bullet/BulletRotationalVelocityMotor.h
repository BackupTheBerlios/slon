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
public:
    BulletRotationalVelocityMotor(BulletConstraint* constraint, int axis);

    // Override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);

    // Override VelocityMotor
    bool enabled() const     { return motor->m_enableMotor; }
    void toggle(bool toggle) { motor->m_enableMotor = toggle; }

    float getTargetVelocity() const         { return motor->m_targetVelocity; }
    void  setTargetVelocity(float velocity) { motor->m_targetVelocity = velocity; }

    float getMaxForce() const       { return motor->m_maxMotorForce; }
    void  setMaxForce(float force)  { motor->m_maxMotorForce = force; }
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_VELOCITY_MOTOR_H__

