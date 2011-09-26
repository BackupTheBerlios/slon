#ifndef __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__
#define __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__

#include "../Realm/EventVisitor.h"
#include "../Scene/Transform.h"
#include "../Scene/AcceptVisitor.hpp"
#include "../Utility/connection.hpp"
#include "Forward.h"

namespace slon {
namespace physics {

class SLON_PUBLIC PhysicsTransform :
	public scene::Transform,
	public scene::AcceptVisitor<realm::EventVisitor>
{
public:
    PhysicsTransform(const collision_object_ptr& collisionObject = collision_object_ptr());

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Node
    void accept(log::LogVisitor& visitor) const;
    void accept(realm::EventVisitor& visitor);

    // Override Transform
    bool isAbsolute() const          { return absolute; }
    void setAbsolute(bool absolute_) { absolute = absolute_; }

    const math::Matrix4f& getTransform() const;
    const math::Matrix4f& getInverseTransform() const;

    /** Set rigid body which handles transform. */
    void setCollisionObject(const collision_object_ptr& collisionObject_);

    /** Get rigid body which handles transformation for this node. */
    CollisionObject* getCollisionObject() { return collisionObject.get(); }

    /** Get rigid body which handles transformation for this node. */
    const CollisionObject* getCollisionObject() const { return collisionObject.get(); }

private:
    void setWorldTransform(const math::RigidTransformr& transform);

protected:
    collision_object_ptr            collisionObject;
    bool                            absolute;
    connection                      transformConnection;
    math::RigidTransformf           transform;
    mutable math::RigidTransformf   invTransform;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__
