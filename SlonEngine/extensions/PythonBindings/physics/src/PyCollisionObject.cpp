#include "stdafx.h"
#include "PyCollisionObject.h"
#include "Physics/CollisionObject.h"

using namespace boost::python;
using namespace slon::physics;

void exportCollisionObject()
{
    scope coScope = 
        class_<CollisionObject, boost::intrusive_ptr<CollisionObject>, boost::noncopyable>("CollisionObject", no_init);

    enum_<CollisionObject::COLLISION_TYPE>("COLLISION_TYPE")
        .value("GHOST",      CollisionObject::CT_GHOST)
        .value("RIGID_BODY", CollisionObject::CT_RIGID_BODY);
}