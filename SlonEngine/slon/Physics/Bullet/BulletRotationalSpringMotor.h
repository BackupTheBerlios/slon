#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SPRING_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SPRING_MOTOR_H__

#define NOMINMAX
#include "../SpringMotor.h"
#include "BulletRotationalMotor.h"
#include "BulletSolver.h"

namespace slon {
namespace physics {

/** Bullet constraint implementation */
class BulletRotationalSpringMotor :
    public BulletRotationalMotor,
    public BulletSolver
{
public:
    BulletRotationalSpringMotor(BulletConstraint* constraint, int axis);

    // override BulletSolver
    void solve(real dt);

    // override SpringMotor
    bool enabled() const                            { enableMotor; }
    void toggle(bool toggle);
    real getMaxForce() const                        { return maxForce; }
    void setMaxForce(real maxForce_)                { maxForce = maxForce_; }
    real getVelocityDamping() const                 { return velocityDamping; }
    void setVelocityDamping(real velocityDamping_)  { velocityDamping = velocityDamping_; }
    real getStiffness() const                       { return stiffness; }
    void setStiffness(real stiffness_)              { stiffness = stiffness_; }
    real getEquilibriumPoint() const                { return equilibrium; }
    void setEquilibriumPoint(real equilibrium_)     { equilibrium = equilibrium_; }

private:
	bool enableMotor;
	real maxForce;
    real equilibrium;
    real stiffness;
    real velocityDamping;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SPRING_MOTOR_H__
