#ifndef __SLON_ENGINE_PHYSICS_BULLET_MOTION_STATE_H__
#define __SLON_ENGINE_PHYSICS_BULLET_MOTION_STATE_H__

#include "../RigidBodyTransform.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace slon {
namespace physics {

class BulletMotionState :
	public btMotionState,
	public RigidBodyTransform
{
public:
	BulletMotionState(const rigid_body_ptr& rigidBody = rigid_body_ptr());
	~BulletMotionState();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	// Override btMotionState
	void getWorldTransform(btTransform &worldTrans) const;
	void setWorldTransform(const btTransform &worldTrans);

private:
	btTransform  worldTransform;
	unsigned int worldTransformTS;
};

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_BULLET_MOTION_STATE_H__
