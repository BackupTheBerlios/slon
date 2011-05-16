#include "stdafx.h"
#define NOMINMAX
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletMotionState.h"

namespace slon {
namespace physics {

BulletMotionState::BulletMotionState(RigidBody* rigidBody)
:	RigidBodyTransform(rigidBody)
,	worldTransformTS(0)
{
}

BulletMotionState::~BulletMotionState()
{
}
	
void BulletMotionState::getWorldTransform(btTransform &worldTrans) const
{
    if (rigidBody->getDynamicsType() == RigidBody::DT_DYNAMIC) {
	    worldTrans = worldTransform;
    }
    else if (rigidBody->getDynamicsType() == RigidBody::DT_STATIC) {
        worldTrans = to_bt_mat( rigidBody->getStateDesc().transform );
    }
    else {
	    worldTrans = to_bt_mat( math::Matrix4r(localToWorld * invTransform) );
    }
}

void BulletMotionState::setWorldTransform(const btTransform &worldTrans)
{
	worldTransform = worldTrans;
	localToWorld = to_mat(worldTrans) * transform;
	//worldToLocal = math::invert(localToWorld);
    update();
}

} // namespace physics
} // namespace slon
