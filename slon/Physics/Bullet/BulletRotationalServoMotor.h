#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__

#define NOMINMAX
#include "../ServoMotor.h"
#include "BulletRotationalMotor.h"

namespace slon {
namespace physics {

class BulletRotationalServoMotor :
    public BulletRotationalMotor<ServoMotor>
{
public:
    BulletRotationalServoMotor(BulletConstraint* constraint, int axis);

    // Override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);

    // Override ServoMotor
    bool  enabled() const                   { return motor->m_enableMotor; }
    float getTargetForce() const            { return targetForce; }
    void  setTargetForce(float targetForce);

private:
    btScalar targetForce;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__

