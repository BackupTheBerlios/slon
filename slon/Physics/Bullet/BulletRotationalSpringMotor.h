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
    void solve(float dt);

    // override BulletRotationalMotor
    void reset(BulletConstraint* constraint, int axis);
    void accept(BulletSolverCollector& collector);

    // override SpringMotor
    bool  enabled() const                            { return motor->m_enableMotor; }
    void  toggle(bool toggle)                        { motor->m_enableMotor = toggle; }
    float getMaxForce() const                        { return motor->m_maxMotorForce; }
    void  setMaxForce(float maxForce)                { motor->m_maxMotorForce = maxForce; }
    float getVelocityDamping() const                 { return velocityDamping; }
    void  setVelocityDamping(float velocityDamping_) { velocityDamping = velocityDamping_; }
    float getStiffness() const                       { return stiffness; }
    void  setStiffness(float stiffness_)             { stiffness = stiffness_; }
    float getEquilibriumPoint() const                { return equilibrium; }
    void  setEquilibriumPoint(float equilibrium_)    { equilibrium = equilibrium_; }

private:
    float   equilibrium;
    float   stiffness;
    float   velocityDamping;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_SPRING_MOTOR_H__
