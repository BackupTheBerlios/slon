#ifndef __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
#define __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__

#include "../Constraint.h"
#include "BulletRotationalMotor.h"
#include <boost/intrusive/slist.hpp>

namespace slon {
namespace physics {

// forwad decl
class BulletDynamicsWorld;
class BulletRigidBody;

/** Bullet constraint implementation */
class BulletConstraint :
    public Constraint
{
friend class BulletDynamicsWorld;
friend class BulletRigidBody;
public:
    typedef boost::shared_ptr<btGeneric6DofConstraint>   bullet_constraint_ptr;
    typedef boost::intrusive_ptr<BulletDynamicsWorld>    dynamics_world_ptr;
    typedef boost::shared_ptr<BulletRotationalMotorBase> rotational_motor_ptr;

private:
    typedef boost::intrusive::slist_member_hook
    < 
        boost::intrusive::link_mode<boost::intrusive::auto_unlink> 
    > slist_hook;

private:
    // non copyable
    BulletConstraint(const BulletConstraint&);
    BulletConstraint& operator = (const BulletConstraint&);

    // Unlink constraint from rigid body
    void unlink(const RigidBody& rigidBody);

    // Destroy constraint
    void destroy();

public:
    BulletConstraint(const bullet_constraint_ptr& constraint,
                     const std::string&           name);
    BulletConstraint(const state_desc& desc);

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    /** Get internal bullet constraint */
    btGeneric6DofConstraint* getBtConstraint() { return constraint.get(); }

    /** Get internal bullet constraint */
    const btGeneric6DofConstraint* getBtConstraint() const { return constraint.get(); }

    // Override BulletObject
    void accept(BulletSolverCollector& collector);

    // Override Constraint
    RigidBody*       getRigidBodyA() const { return desc.rigidBodies[0]; }
    RigidBody*       getRigidBodyB() const { return desc.rigidBodies[1]; }
    
    const Motor*     getMotor(Motor::TYPE motor) const;
    Motor*           getMotor(Motor::TYPE motor);
    ServoMotor*      createServoMotor(Motor::TYPE motor);
    VelocityMotor*   createVelocityMotor(Motor::TYPE motor);
    SpringMotor*     createSpringMotor(Motor::TYPE motor);

    math::Vector3r   getAxis(unsigned int axis) const;
    real             getRotationAngle(unsigned int axis) const;
    AXIS_RESTRICTION getAxisRestriction(unsigned int axis) const;

    const std::string&  getName() const      { return desc.name; }
    const state_desc&   getStateDesc() const { return desc; }

    void reset(const state_desc& desc);

    ~BulletConstraint();

private:
    state_desc              desc;
    bullet_constraint_ptr   constraint;
    rotational_motor_ptr    rotationalMotors[3];
    dynamics_world_ptr      dynamicsWorld;
    slist_hook              dynamicsWorldHook; // to store in dynamics World
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_CONSTRAINT_H__
