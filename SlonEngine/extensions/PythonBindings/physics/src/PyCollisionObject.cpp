#include "stdafx.h"
#include "PyCollisionObject.h"
#include "Physics/CollisionObject.h"

using namespace boost::python;
using namespace slon::physics;

boost::shared_ptr<math::Matrix4r> CollisionObjectGetTransform(const CollisionObject* co)
{
    return boost::shared_ptr<math::Matrix4r>( new math::Matrix4r(co->getTransform()) );
}

void exportCollisionObject()
{
    scope coScope = 
        class_<CollisionObject, boost::intrusive_ptr<CollisionObject>, boost::noncopyable>("CollisionObject", no_init)
            .add_property("name",      make_function(&CollisionObject::getName, return_internal_reference<>()))
            .add_property("type",      &CollisionObject::getType)
            .def("getTransform",       CollisionObjectGetTransform, with_custodian_and_ward_postcall<1,0>())
            .def("setTransform",       &CollisionObject::setTransform);

    enum_<CollisionObject::COLLISION_TYPE>("COLLISION_TYPE")
        .value("GHOST",      CollisionObject::CT_GHOST)
        .value("RIGID_BODY", CollisionObject::CT_RIGID_BODY);
}