#include "stdafx.h"
#include "Physics/Bullet/BulletCollisionObject.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"

namespace slon {
namespace physics {

BulletCollisionObject::BulletCollisionObject(CollisionObject* pInterface_, BulletDynamicsWorld* dynamicsWorld_) 
:   pInterface(pInterface_)
,   dynamicsWorld(dynamicsWorld_)
{
}

BulletCollisionObject::~BulletCollisionObject()
{
}

} // namespace slon
} // namespace physics
