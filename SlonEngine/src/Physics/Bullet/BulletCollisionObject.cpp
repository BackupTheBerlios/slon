#include "stdafx.h"
#include "Physics/Bullet/BulletCollisionObject.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"

namespace slon {
namespace physics {

BulletCollisionObject::BulletCollisionObject(CollisionObject* pInterface_, DynamicsWorld* dynamicsWorld_) 
:   pInterface(pInterface_)
,   dynamicsWorld( dynamicsWorld_->getImpl() )
{
}

BulletCollisionObject::~BulletCollisionObject()
{
}

CollisionObject::connection_type BulletCollisionObject::connectContactAppearCallback(const CollisionObject::contact_handler& handler)
{
    return contactAppearSignal.connect(handler); 
}

CollisionObject::connection_type BulletCollisionObject::connectContactDissapearCallback(const CollisionObject::contact_handler& handler)
{
    return contactDissapearSignal.connect(handler); 
}

void BulletCollisionObject::handleAppearingContact(const Contact& contact)
{
    contactAppearSignal(contact);
}

void BulletCollisionObject::handleDissappearingContact(const Contact& contact)
{
    contactDissapearSignal(contact);
}

} // namespace slon
} // namespace physics
