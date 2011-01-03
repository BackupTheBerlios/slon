#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_SKELETON_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_SKELETON_H__

#include "../Skeleton.h"
#include "../../Physics/RigidBody.h"

namespace slon {
namespace scene {

/** This bone can't be directly transformed. It is handled
 * by the physics engine. You can apply forces, torques and impulses
 * to the bones.
 */
class PhysicsBone :
    public Bone
{
public:
    typedef scene::Bone     base_type;
    typedef PhysicsBone     this_type;

public:
    /** Attach rigid body to the bone */
    void setRigidBody(physics::RigidBody* _rigidBody) { rigidBody.reset(_rigidBody); }

    /** Setup end of the bone in the local coordinates. */
    void setBoneEndJoint(const math::Vector3f& _boneEnd) { boneEnd = _boneEnd; }

    /** Setup start of the bone in the local coordinates. */
    void setBoneStartJoint(const math::Vector3f& _boneStart) { boneStart = _boneStart; }

    /** Get rigid body attached to the bone */
    const physics::RigidBody* getRigidBody() const { return rigidBody.get(); }

    /** Apply local torque to the bone. Actually applies force to the tip of the bone
     * divided by the bone length. */
    void applyTorque(const math::Vector3f& torque);

    // unhide accept
    using Node::accept;

    // Override node
    void accept(scene::TraverseVisitor& visitor);

    // Override Bone
    const math::Quaternionf& getRotation() const;
    const math::Vector3f&    getTranslation() const;

    // unhide transform functions
    using Transform::getTransformMatrix;
    using Transform::getInverseTransformMatrix;

    // Override transform
    math::Matrix4f& getTransformMatrix(math::Matrix4f& out) const;
    math::Matrix4f& getInverseTransformMatrix(math::Matrix4f& out) const;

    // always modified
    unsigned int getModifiedCount() const
    {
        static unsigned int modifiedCount = 0;
        return ++modifiedCount;
    }

    virtual ~PhysicsBone() {}

private:
    // transform
    math::Matrix4f      localTransform;
    math::Quaternionf   rotation;
    math::Vector3f      translation;
    math::Vector3f      boneStart;
    math::Vector3f      boneEnd;

    // physics
    physics::rigid_body_ptr rigidBody;
};

typedef boost::intrusive_ptr<PhysicsBone>           physics_bone_ptr;
typedef boost::intrusive_ptr<const PhysicsBone>     const_physics_bone_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_SKELETON_H__
