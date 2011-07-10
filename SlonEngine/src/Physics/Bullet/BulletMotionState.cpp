#include "stdafx.h"
#define NOMINMAX
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletMotionState.h"

namespace slon {
namespace physics {

BulletMotionState::BulletMotionState(const rigid_body_ptr& rigidBody)
:	RigidBodyTransform(rigidBody)
,	worldTransformTS(0)
{
}

BulletMotionState::~BulletMotionState()
{
}
	
const char* BulletMotionState::serialize(database::OArchive& ar) const
{
	// serialize base class
	RigidBodyTransform::serialize(ar);

	// serialize data
    return "BulletMotionState";
}

void BulletMotionState::deserialize(database::IArchive& ar)
{
	// deserialize base class
	MatrixTransform::deserialize(ar);

	// deserialize data
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

void BulletMotionState::accept(realm::EventVisitor& ev)
{
    if (ev.getType() == realm::EventVisitor::WORLD_ADD) {
        rigidBody->toggleSimulation(true);
    }
    else if (ev.getType() == realm::EventVisitor::WORLD_REMOVE) {
        rigidBody->toggleSimulation(false);
    }
}

} // namespace physics
} // namespace slon
