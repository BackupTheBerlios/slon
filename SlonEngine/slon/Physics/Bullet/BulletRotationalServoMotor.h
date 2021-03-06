#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__

#define NOMINMAX
#include "../ServoMotor.h"
#include "BulletRotationalMotor.h"
#include "BulletSolver.h"

namespace slon {
namespace physics {

class BulletRotationalServoMotor :
    public BulletRotationalMotor,
	public BulletSolver
{
public:
    BulletRotationalServoMotor(BulletConstraint* constraint, int axis);
    ~BulletRotationalServoMotor();

    // Override BulletSolver
    void solve(real dt);

    // Override ServoMotor
    bool enabled() const                   { return enableMotor; }
    real getTargetForce() const            { return targetForce; }
    void setTargetForce(real targetForce);

private:
    real targetForce;
    bool enableMotor;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SERVO_MOTOR_H__

