#include "stdafx.h"
#include "Physics/Bullet/BulletCollisionObject.h"
#include "Physics/RigidBody.h"

namespace slon {
namespace physics {

template<typename Base>
BulletCollisionObject<Base>::BulletCollisionObject(DynamicsWorld* dynamicsWorld_) :
	dynamicsWorld( static_cast<BulletDynamicsWorld*>(dynamicsWorld_) )
{
}

template<typename Base>
BulletCollisionObject<Base>::~BulletCollisionObject()
{
}

// explicit instantiation
template class BulletCollisionObject<RigidBody>;

} // namespace slon
} // namespace physics
