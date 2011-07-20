#include "stdafx.h"
#include "Physics/RigidBody.h"
#ifdef SLON_ENGINE_USE_BULLET
#   include "Physics/Bullet/BulletRigidBody.h"
#endif

namespace slon {
namespace physics {

CollisionObject::impl_type* CollisionObject::getImpl()
{
    if (getType() == CT_RIGID_BODY) {
        return static_cast<RigidBody*>(this)->getImpl();
    }

    return 0;
}

const CollisionObject::impl_type* CollisionObject::getImpl() const
{
    if (getType() == CT_RIGID_BODY) {
        return static_cast<const RigidBody*>(this)->getImpl();
    }

    return 0;
}

} // namespace physics
} // namespace slon
