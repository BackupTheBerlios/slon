#include "stdafx.h"
#include "PyCollisionObject.h"
#include "Physics/CollisionObject.h"
#include "Physics/PhysicsTransform.h"

using namespace boost::python;
using namespace slon::physics;
using namespace slon::scene;

void exportPhysicsTransform()
{
    // resolve ambiguities
    CollisionObject* (PhysicsTransform::*GetCollisionObject)() = &PhysicsTransform::getCollisionObject;

    class_<PhysicsTransform, bases<Transform>, boost::intrusive_ptr<PhysicsTransform>, boost::noncopyable>("PhysicsTransform", no_init)
        .def(init<const collision_object_ptr&>())
        .add_property("absolute",        &PhysicsTransform::isAbsolute,                                    &PhysicsTransform::setAbsolute)
        .add_property("collisionObject", make_function(GetCollisionObject, return_internal_reference<>()), &PhysicsTransform::setCollisionObject);
    implicitly_convertible< boost::intrusive_ptr<PhysicsTransform>,         boost::intrusive_ptr<Transform> >();
    implicitly_convertible< boost::intrusive_ptr<PhysicsTransform>,         boost::intrusive_ptr<const Transform> >();
    implicitly_convertible< boost::intrusive_ptr<const PhysicsTransform>,   boost::intrusive_ptr<const Transform> >();
}