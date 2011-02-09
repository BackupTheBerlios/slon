#ifndef __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__
#define __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__

#define NOMINMAX
#include "../Motor.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace slon {
namespace physics {

// forward
class BulletConstraint;
class BulletSolverCollector;

class BulletRotationalMotorBase
{
public:
    BulletRotationalMotorBase(BulletConstraint* constraint, int axis);

    virtual void accept(BulletSolverCollector& collector) {}

    /** Replace internal bullet motor */
    virtual void reset(BulletConstraint* constraint, int axis);

    /** Get internal bullet motor */
    virtual btRotationalLimitMotor* getBtMotor() { return motor; }

    /** Get internal bullet motor */
    virtual const btRotationalLimitMotor* getBtMotor() const { return motor; }

    /** Get pointer to motor interface */
    virtual Motor* asMotor() = 0;

    /** Get pointer to motor interface */
    virtual const Motor* asMotor() const = 0;

protected:
    BulletConstraint*       constraint;
    int                     axis;
    btRotationalLimitMotor* motor;
};

template<typename Base>
class BulletRotationalMotor :
    public Base,
    public BulletRotationalMotorBase
{
public:
    BulletRotationalMotor(BulletConstraint* constraint, int axis);

    // override BulletRotationalMotorBase
    void         reset(BulletConstraint* constraint, int axis);
    Motor*       asMotor()       { return this; }
    const Motor* asMotor() const { return this; }

    // Gather velocity/force information
    void accept(BulletSolverCollector& collector);

    // override Base
    const Constraint*  getConstraint() const;
    math::Vector3r     getAxis() const;
    Motor::TYPE        getType() const       { return Motor::TYPE(Motor::MOTOR_X_ROT + axis); }
    real               getLoLimit() const    { return motor->m_loLimit; }
    real               getHiLimit() const    { return motor->m_hiLimit; }
    real               getPosition() const   { return motor->m_currentPosition; }
    real               getVelocity() const   { return velocity; }
    real               getForce() const      { return force; }

protected:
    btScalar    velocity;
    btScalar    force;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_ROTATIONAL_MOTOR_H__