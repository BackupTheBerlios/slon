#ifndef __SLON_ENGINE_SCENE_PHYSICS_RIGID_BODY_TRANSFORM_H__
#define __SLON_ENGINE_SCENE_PHYSICS_RIGID_BODY_TRANSFORM_H__

#include "../../Physics/RigidBody.h"
#include "../Transform.h"

namespace slon {
namespace scene {

class RigidBodyTransform :
    public Transform
{
public:
    // unhide accept functions
    using Node::accept;

    // unhide transform functions
    using Transform::getTransform;
    using Transform::getInverseTransform;

public:
    RigidBodyTransform(physics::RigidBody* rigidBody = 0);

    // Override node
    void accept(scene::TraverseVisitor& visitor);

    // Override transform
    const math::Matrix4f& getTransform() const;
    const math::Matrix4f& getInverseTransform() const;
    unsigned int          getModifiedTS() const { return ++modifiedCount; }

    /** Setup rigid body which will handle transformation for this node. */
    void setRigidBody(physics::RigidBody* rigidBody);

    /** Get rigid body which handles transformation for this node. */
    physics::RigidBody* getRigidBody() { return rigidBody.get(); }

    /** Setup scaling removed from rigid body transform */
    void setScaling(const math::Vector3f& _scaling) { scaling = _scaling; }

    /** Get post scaling. */
    const math::Vector3f& getScaling() const { return scaling; } 

private:
    physics::rigid_body_ptr rigidBody;
    math::Vector3f          scaling;
    mutable unsigned int    modifiedCount;
};

typedef boost::intrusive_ptr<RigidBodyTransform>        rigid_body_transform_ptr;
typedef boost::intrusive_ptr<const RigidBodyTransform>  const_rigid_body_transform_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_PHYSICS_RIGID_BODY_TRANSFORM_H__
