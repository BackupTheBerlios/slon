#ifndef __SLON_ENGINE_PHYSICS_RIGID_BODY_H__
#define __SLON_ENGINE_PHYSICS_RIGID_BODY_H__

#include "CollisionObject.h"
#include "CollisionShape.h"
#include "RigidBodyTransform.h"

namespace slon {
namespace physics {

class Constraint;

/** Rigid body in the physics world */
class RigidBody :
    public CollisionObject
{
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
       : public sgl::Aligned16
#endif
    {
        math::Matrix4r  transform;          /// initial transformation matrix
        DYNAMICS_TYPE   type;
        real            mass;
        real            margin;             /// collision margin
        real            relativeMargin;     /// collision margin relative to the lowest dimension of the collision shape (e.g. box side, sphere radius, cylinder height)
        math::Vector3r  linearVelocity;
        math::Vector3r  angularVelocity;
        std::string     name;               /// name of the rigid body
        std::string     target;             /// name of the scene graph node which targets this rigid body

        // collision shapes
        const_collision_shape_ptr collisionShape;

        state_desc(const std::string& _name = "") :
            transform( math::make_identity<float, 4>() ),
            mass(0),
            margin(0),
            relativeMargin( real(0.02) ),
            linearVelocity(0, 0, 0),
            angularVelocity(0, 0, 0),
            name(_name)
        {}
    };

public:
    /** Apply force to rigid body at the specified position. */
    virtual void applyForce(const math::Vector3r& force, const math::Vector3r& pos) = 0;

    /** Apply torque to rigid body. */
    virtual void applyTorque(const math::Vector3r& torque) = 0;

    /** Apply impulse to rigid body. */
    virtual void applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos) = 0;

    /** Apply toque impulse to rigid body. */
    virtual void applyTorqueImpulse(const math::Vector3r& torqueImpulse) = 0;

    /** Get sum of all forces applied to the body. */
    virtual math::Vector3r getTotalForce() const = 0;

    /** Get sum of all torques applied to the body. */
    virtual math::Vector3r getTotalTorque() const = 0;

    /** Get mass of the rigid body */
    virtual real getMass() const = 0;

    /** Get activation/deactivation state/policy of the object */
    virtual ACTIVATION_STATE getActivationState() const = 0;

    /** Set activation/deactivation state/policy of the object */
    virtual void setActivationState(ACTIVATION_STATE state) = 0;

    /** Get linear velocity of the body */
    virtual math::Vector3r getLinearVelocity() const = 0;

    /** Get angular velocity of the body. 
     * @return angular velocity in axis angle representation: direction of the vector is
     * the rotation axis, length of the vector is angular velocity.
     */
    virtual math::Vector3r getAngularVelocity() const = 0;

    /** Get rigid body graphics target */
    virtual const std::string& getTarget() const = 0;

    /** Get rigid body name */
    virtual const std::string& getName() const = 0;

    /** Get dynamics tpye of the object. */
    virtual DYNAMICS_TYPE getDynamicsType() const = 0;

    /** Get rigid body description structure state. */
    virtual const state_desc& getStateDesc() const = 0;

	/** Get motion state of rigid body for tracking its motion.
	 * For dynamic rigid body motion state will treat rigid body transform as absolute transform for scene graph, for
	 * kinematic it will setup scene graph transform as rigid body transform.
	 * Use matrix to specify transition between rigidBody and scene frames.
	 */
	virtual RigidBodyTransform* getMotionState() = 0;

	/** Get motion state of rigid body for tracking its motion.
	 * For dynamic rigid body motion state will treat rigid body transform as absolute transform for scene graph, for
	 * kinematic it will setup scene graph transform as rigid body transform.
	 * Use matrix to specify transition between rigidBody and scene frames.
	 */
	virtual const RigidBodyTransform* getMotionState() const = 0;

    /** Recreate rigid body from desc */
    virtual void reset(const state_desc& desc) = 0;

    /** Setup collision shape for the rigid body. Deletes old rigid body and
     * creates new with the new collision shape and old rigid body state. You
     * can setup NULL collision shape, if so rigid body will be removed from the
     * world until you provide valid collision shape for it.
     *
    void setCollisionShape(CollisionShape* shape);*/

    /** Check whether rigid body has influence in the world. *
    bool isInWorld() const; */

    /** Remove rigid body from the world for a while *
    void removeFromWorld(); */

    /** Return rigid body to the world.
     * @return true - if success.
     *
    bool addInWorld(); */

    /** Get iterator addressing first constraint attached to this rigid body */
    virtual constraint_iterator firstConstraint() = 0;

    /** Get iterator addressing end of the constraints */
    virtual constraint_iterator endConstraint() = 0;

    virtual ~RigidBody() {}
};

// ptr typedef
typedef boost::intrusive_ptr<RigidBody>         rigid_body_ptr;
typedef boost::intrusive_ptr<const RigidBody>   const_rigid_body_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_RIGID_BODY_H__
