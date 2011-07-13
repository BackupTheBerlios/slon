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
typedef BulletRotationalMotor<ServoMotor> motor_base;
public:
    BulletRotationalServoMotor(BulletConstraint* constraint, int axis);

    // Override BulletSolver
    void solve(real dt);
    void accept(BulletSolverCollector& collector);

    // Override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);

    // Override ServoMotor
    bool enabled() const                   { return enableMotor; }
    real getTargetForce() const            { return targetForce; }
    void setTargetForce(real targetForce);

private:
    btScalar targetForce;
    bool     enableMotor;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__

