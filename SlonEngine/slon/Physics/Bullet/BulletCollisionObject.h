#ifndef __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__
#define __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__

#include "../CollisionObject.h"
#include "../DynamicsWorld.h"
#include "BulletDynamicsWorld.h"

namespace slon {
namespace physics {

class BulletCollisionObject
{
private:
    typedef boost::signal<void (const Contact&)>        contact_signal;
    typedef boost::intrusive_ptr<BulletDynamicsWorld>   dynamics_world_ptr;

public:
    BulletCollisionObject(CollisionObject* pInterface, DynamicsWorld* dynamicsWorld);
    virtual ~BulletCollisionObject();

    // Override CollisionObject
    const DynamicsWorld& getDynamicsWorld() const { return *dynamicsWorld; }

    CollisionObject::connection_type connectContactAppearCallback(const CollisionObject::contact_handler& handler)
    {
        return contactAppearSignal.connect(handler); 
    }

    CollisionObject::connection_type connectContactDissapearCallback(const CollisionObject::contact_handler& handler)
    {
        return contactDissapearSignal.connect(handler); 
    }

    void handleAppearingContact(const Contact& contact)
    {
        contactAppearSignal(contact);
    }

    void handleDissappearingContact(const Contact& contact)
    {
        contactDissapearSignal(contact);
    }

protected:
	CollisionObject*        pInterface;
    contact_signal          contactAppearSignal;
    contact_signal          contactDissapearSignal;
    dynamics_world_ptr      dynamicsWorld;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__
