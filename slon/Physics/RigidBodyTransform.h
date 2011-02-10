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
    RigidBodyTransform(physics::RigidBody* rigidBody = 0);

    /** Get rigid body which handles transformation for this node. */
    physics::RigidBody* getRigidBody() { return rigidBody.get(); }

    /** Get rigid body which handles transformation for this node. */
    const physics::RigidBody* getRigidBody() const { return rigidBody.get(); }

protected:
    physics::rigid_body_ptr rigidBody;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_RIGID_BODY_TRANSFORM_H__
