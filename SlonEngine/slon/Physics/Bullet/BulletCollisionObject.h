#ifndef __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__
#define __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__

#include "../CollisionObject.h"

namespace slon {
namespace physics {

class BulletDynamicsWorld;

class BulletCollisionObject :
	public boost::noncopyable
{
friend class BulletMotionState;
public:
    typedef signal<void (const math::Matrix4f&)>  transform_signal;
    typedef signal<void (const Contact&)>         contact_signal;

public:
    BulletCollisionObject(CollisionObject* pInterface, BulletDynamicsWorld* dynamicsWorld);
    virtual ~BulletCollisionObject();

    // Implement CollisionObject
    transform_signal&   getTransformSignal()         { return pInterface->transformSignal; }
    contact_signal&     getContactAppearSignal()     { return pInterface->contactAppearSignal; }
    contact_signal&     getContactDissapearSignal()  { return pInterface->contactDissapearSignal; }
	
	/** Get interface handling this implementation object. */
	CollisionObject* getInterface() { return pInterface; }

protected:
	CollisionObject*        pInterface;
    BulletDynamicsWorld*    dynamicsWorld;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_BULLET_BULLET_COLLISION_OBJECT_H__
