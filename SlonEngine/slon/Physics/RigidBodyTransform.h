#ifndef __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__
#define __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__

#include "../Scene/MatrixTransform.h"
#include "RigidBody.h"

namespace slon {
namespace physics {

class RigidBodyTransform :
	public scene::MatrixTransform
{
public:
    RigidBodyTransform(const rigid_body_ptr& rigidBody = rigid_body_ptr());

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Node
    void accept(log::LogVisitor& visitor) const;

    // Override Transform
    bool isAbsolute() const          { return absolute; }
    void setAbsolute(bool absolute_) { absolute = absolute_; }

    /** Set rigid body which handles transform. */
    void setRigidBody(const rigid_body_ptr& rigidBody_) { rigidBody = rigidBody_; }

    /** Get rigid body which handles transformation for this node. */
    RigidBody* getRigidBody() { return rigidBody.get(); }

    /** Get rigid body which handles transformation for this node. */
    const RigidBody* getRigidBody() const { return rigidBody.get(); }

protected:
    bool            absolute;
    rigid_body_ptr	rigidBody;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__
