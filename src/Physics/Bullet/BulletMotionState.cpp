#include "stdafx.h"
#define NOMINMAX
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletMotionState.h"
#include "Scene/Visitors/TraverseVisitor.h"

namespace slon {
namespace physics {

BulletMotionState::BulletMotionState(RigidBody* rigidBody)
:	RigidBodyTransform(rigidBody)
,	worldTransformTS(0)
{
}
	
void BulletMotionState::accept(scene::TraverseVisitor& visitor)
{
	if (rigidBody->getDynamicsType() == RigidBody::DT_DYNAMIC) {
		visitor.visitAbsoluteTransform(*this);
	}
	else {
		worldTransform = to_bt_mat( math::Matrix4r(localToWorld * transform) );
	}
}

void BulletMotionState::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans = worldTransform;
}

void BulletMotionState::setWorldTransform(const btTransform &worldTrans)
{
	worldTransform = worldTrans;
	localToWorld = to_mat(worldTrans) * transform;
	//worldToLocal = math::invert(localToWorld);
	++modifiedCount;
}

} // namespace physics
} // namespace slon
