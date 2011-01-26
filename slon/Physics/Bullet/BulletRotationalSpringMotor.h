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
    public BulletRotationalMotor<SpringMotor>,
    public BulletSolver
{
public:
    BulletRotationalSpringMotor(BulletConstraint* constraint, int axis);

    // override BulletSolver
    void solve(real dt);

    // override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);
    void accept(BulletSolverCollector& collector);

    // override SpringMotor
    bool enabled() const                            { return motor->m_enableMotor; }
    void toggle(bool toggle)                        { motor->m_enableMotor = toggle; }
    real getMaxForce() const                        { return motor->m_maxMotorForce; }
    void setMaxForce(real maxForce)                 { motor->m_maxMotorForce = maxForce; }
    real getVelocityDamping() const                 { return velocityDamping; }
    void setVelocityDamping(real velocityDamping_)  { velocityDamping = velocityDamping_; }
    real getStiffness() const                       { return stiffness; }
    void setStiffness(real stiffness_)              { stiffness = stiffness_; }
    real getEquilibriumPoint() const                { return equilibrium; }
    void setEquilibriumPoint(real equilibrium_)     { equilibrium = equilibrium_; }

private:
    real   equilibrium;
    real   stiffness;
    real   velocityDamping;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SPRING_MOTOR_H__
