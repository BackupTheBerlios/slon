#ifndef __SLON_ENGINE_PHYSICS_RIGID_BODY_H__
#define __SLON_ENGINE_PHYSICS_RIGID_BODY_H__

#include "CollisionObject.h"
#include "CollisionShape.h"
#ifdef SLON_ENGINE_USE_SSE
#   include "../Utility/Memory/aligned.hpp"
#endif

namespace slon {
namespace physics {

class Constraint;

#ifdef SLON_ENGINE_USE_BULLET
    class BulletRigidBody;
#endif

/** Rigid body in the physics world */
class RigidBody :
    public CollisionObject
{
private:
	friend class Constraint;
    friend class DynamicsWorld;
#ifdef SLON_ENGINE_USE_BULLET
    friend class BulletRigidBody;
	typedef BulletRigidBody              impl_type;
    typedef boost::scoped_ptr<impl_type> impl_ptr;
#endif
public:
    typedef std::vector<Constraint*>    constraint_vector;
    typedef constraint_vector::iterator constraint_iterator;

public:
    enum DYNAMICS_TYPE
    {
        DT_STATIC,     /// Static objects are fixed in the world and collide with other obejcts.
        DT_KINEMATIC,  /// Kinematic objects collide with other objects but they have no influence on kinematic objects.
        DT_DYNAMIC     /// Rigid bodies interact with all other collision objects.
    };

    enum ACTIVATION_STATE
    {
        AS_ACTIVE,
        AS_SLEEPING,
        AS_DISABLE_DEACTIVATION,
        AS_DISABLE_SIMULATION
    };

    struct state_desc
#ifdef SLON_ENGINE_USE_SSE
       : public aligned<0x10>
#endif
    {
        math::Matrix4r            transform;          /// initial transformation matrix
        DYNAMICS_TYPE             type;
        real                      mass;
        math::Vector3r            inertia;            /// inertia tensor
        real                      margin;             /// collision margin
        real                      relativeMargin;     /// collision margin relative to the lowest dimension of the collision shape (e.g. box side, sphere radius, cylinder height)
        math::Vector3r            linearVelocity;
        math::Vector3r            angularVelocity;
        std::string               name;               /// name of the rigid body
        const_collision_shape_ptr collisionShape;

        state_desc(const std::string& _name = "") :
            transform( math::make_identity<float, 4>() ),
            type(DT_DYNAMIC),
            mass(0),
            inertia(0, 0, 0),
            margin(0),
            relativeMargin( real(0.02) ),
            linearVelocity(0, 0, 0),
            angularVelocity(0, 0, 0),
            name(_name)
        {}
    };

public:
	RigidBody(const state_desc& desc = state_desc());

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	// Override CollisionObject
	COLLISION_TYPE        getType() const;
    const CollisionShape* getCollisionShape() const;
    const DynamicsWorld*  getDynamicsWorld() const;
    const std::string&    getName() const;
    math::Matrix4r        getTransform() const;
    void                  setTransform(const math::Matrix4r& transform);

    /** Apply force to rigid body at the specified position. */
    void applyForce(const math::Vector3r& force, const math::Vector3r& pos);

    /** Apply torque to rigid body. */
    void applyTorque(const math::Vector3r& torque);

    /** Apply impulse to rigid body. */
    void applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos);

    /** Apply toque impulse to rigid body. */
    void applyTorqueImpulse(const math::Vector3r& torqueImpulse);

    /** Get sum of all forces applied to the body. */
    math::Vector3r getTotalForce() const;

    /** Get sum of all torques applied to the body. */
    math::Vector3r getTotalTorque() const;

    /** Get mass of the rigid body */
    real getMass() const;

    /** Get local inertia tensor of rigid body */
    math::Vector3r getInertiaTensor() const;

    /** Get activation/deactivation state/policy of the object */
    ACTIVATION_STATE getActivationState() const;

    /** Set activation/deactivation state/policy of the object */
    void setActivationState(ACTIVATION_STATE state);

    /** Get linear velocity of the body */
    math::Vector3r getLinearVelocity() const;

    /** Get angular velocity of the body. 
     * @return angular velocity in axis angle representation: direction of the vector is
     * the rotation axis, length of the vector is angular velocity.
     */
    math::Vector3r getAngularVelocity() const;

    /** Get dynamics type of the object. */
    DYNAMICS_TYPE getDynamicsType() const;

    /** Get rigid body description structure state. */
    const state_desc& getStateDesc() const;

    /** Recreate rigid body from desc */
    void reset(const state_desc& desc);

    /** Get iterator addressing first constraint attached to this rigid body */
    constraint_iterator firstConstraint();

    /** Get iterator addressing end of the constraints */
    constraint_iterator endConstraint();

	/** Get implementation object. */
	impl_type* getImpl() { return impl.get(); }

	/** Get implementation object. */
	const impl_type* getImpl() const { return impl.get(); }

private:
    /** Add rigid body into the world, should be called by DynamicsWorld. */
    void setWorld(const dynamics_world_ptr& world);

	/** Instantiate implementation. */
	void instantiate();

	/** Release implementation. */
	void release();

	/** Add constraint affecting this rigid body, should be called by Constraint. */
	void addConstraint(Constraint* constraint);
	
	/** Remove constraint affecting this rigid body, should be called by Constraint. */
	void removeConstraint(Constraint* constraint);

private:
    dynamics_world_ptr world;
    mutable state_desc desc;
    impl_ptr           impl;
    constraint_vector  constraints;
};

// ptr typedef
typedef boost::intrusive_ptr<RigidBody>         rigid_body_ptr;
typedef boost::intrusive_ptr<const RigidBody>   const_rigid_body_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_RIGID_BODY_H__
