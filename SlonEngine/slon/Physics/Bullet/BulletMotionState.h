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
	BulletMotionState(RigidBody* rigidBody);
	~BulletMotionState();

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
