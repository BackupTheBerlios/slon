#ifndef __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__
#define __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__

#include "../RigidBody.h"
#include "BulletCollisionObject.h"
#include <btBulletDynamicsCommon.h>

namespace slon {
namespace physics {

class BulletRigidBody :
    public BulletCollisionObject<RigidBody>
{
friend class BulletConstraint;
public:
    typedef BulletCollisionObject<RigidBody>                base_type;

	typedef boost::shared_ptr<btMotionState>                motion_state_ptr;
	typedef boost::shared_ptr<btRigidBody>                  rigid_body_ptr;

	typedef boost::intrusive_ptr<CollisionShape>            collision_shape_ptr;
    typedef boost::intrusive_ptr<BulletDynamicsWorld>	    dynamics_world_ptr;

    typedef boost::signal<void (const BulletRigidBody&)>    rigid_body_signal;
    typedef boost::signals::connection                      connection_type;

private:
	// non copyable
	BulletRigidBody(const BulletRigidBody&);
	BulletRigidBody& operator = (const BulletRigidBody&);

    // destroy helper objects
    void destroy(bool unlinkConstraints = true);

    // remove constraint reference
    void removeConstraint(const Constraint& constraint);

public:
    BulletRigidBody(const rigid_body_ptr rigidBody, 
                    const std::string&   name,
                    DynamicsWorld*       dynamicsWorld);

    BulletRigidBody(const RigidBody::state_desc& desc, 
                    DynamicsWorld*               dynamicsWorld);

    // implement RigidBody
    void applyForce(const math::Vector3f& force, const math::Vector3f& pos);
    void applyTorque(const math::Vector3f& torque);
    void applyImpulse(const math::Vector3f& impulse, const math::Vector3f& pos);
    void applyTorqueImpulse(const math::Vector3f& torqueImpulse);

    math::Vector3f      getTotalForce() const;
    math::Vector3f      getTotalTorque() const;
    math::Matrix4f      getTransform() const;
    float 		        getMass() const;
    ACTIVATION_STATE    getActivationState() const;
    math::Vector3f      getLinearVelocity() const;
    math::Vector3f      getAngularVelocity() const;

    COLLISION_TYPE                  getCollisionType() const    { return CT_RIGID_BODY; }
    const CollisionShape* 	        getCollisionShape() const   { return desc.collisionShape.get(); }
    const DynamicsWorld&	        getDynamicsWorld() const    { return *dynamicsWorld; }
    const std::string&              getTarget() const           { return desc.target; }
    const std::string&              getName() const             { return desc.name; }
    RigidBody::DYNAMICS_TYPE        getDynamicsType() const     { return desc.type; }
    const RigidBody::state_desc&    getStateDesc() const        { return desc; }

    void    reset(const RigidBody::state_desc& desc);
    void    setTransform(const math::Matrix4f& worldTransform);
    void    setActivationState(ACTIVATION_STATE state);

    RigidBody::constraint_iterator firstConstraint()    { return constraints.begin(); }
    RigidBody::constraint_iterator endConstraint()      { return constraints.end(); }

    /** Connect event handler that will be called on reset. */
    connection_type connectResetHandler(rigid_body_signal::slot_type handler);

    /** Connect event handler that will be called on destroy. */
    connection_type connectDestroyHandler(rigid_body_signal::slot_type handler);

    /** Get internal bullet rigid body. */
    const rigid_body_ptr& getBtRigidBody() const { return rigidBody; }

    ~BulletRigidBody();

private:
    RigidBody::state_desc 	        desc;
    RigidBody::constraint_vector    constraints;

    // bullet
    motion_state_ptr    motionState;
    rigid_body_ptr      rigidBody;

    // handlers
    rigid_body_signal   onResetSignal;
    rigid_body_signal   onDestroySignal;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_RIGID_BODY_H__
