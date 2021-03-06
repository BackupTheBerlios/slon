#ifndef __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__
#define __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__

#include "../RigidBody.h"
#include "BulletCollisionObject.h"

class btMotionState;
class btRigidBody;

namespace slon {
namespace physics {

class BulletMotionState;

class BulletRigidBody :
	public BulletCollisionObject
{
friend class BulletConstraint;
public:
    typedef BulletCollisionObject                           base_type;

	typedef boost::scoped_ptr<BulletMotionState>            motion_state_ptr;
	typedef boost::scoped_ptr<btRigidBody>                  rigid_body_ptr;

    typedef boost::signal<void (const BulletRigidBody&)>    rigid_body_signal;
    typedef boost::signals::connection                      connection_type;

public:
    BulletRigidBody(RigidBody*           pInterface,
                    BulletDynamicsWorld* dynamicsWorld,
                    btRigidBody*         rigidBody, 
                    const std::string&   name );
    BulletRigidBody(RigidBody*           pInterface,
                    BulletDynamicsWorld* dynamicsWorld);
    ~BulletRigidBody();

    // Implement RigidBody
    void applyForce(const math::Vector3r& force, const math::Vector3r& pos);
    void applyTorque(const math::Vector3r& torque);
    void applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos);
    void applyTorqueImpulse(const math::Vector3r& torqueImpulse);

    math::Vector3r				    getTotalForce() const;
    math::Vector3r				    getTotalTorque() const;
    const math::RigidTransformr*	getTransformPointer() const;
    const math::RigidTransformr&	getTransform() const;
    RigidBody::ACTIVATION_STATE     getActivationState() const;
    math::Vector3r				    getLinearVelocity() const;
    math::Vector3r				    getAngularVelocity() const;

    void    setTransform(const math::Matrix4r& worldTransform);
    void    setActivationState(RigidBody::ACTIVATION_STATE state);
    void    toggleSimulation(bool toggle);

    /** Get internal bullet rigid body. */
    btRigidBody& getBtRigidBody() const { return (*rigidBody); }

	/** Get interface handling this implementation object. */
    RigidBody* getInterface() { return pInterface; }

private:
    RigidBody*          pInterface;
    motion_state_ptr    motionState;
    rigid_body_ptr      rigidBody;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__
