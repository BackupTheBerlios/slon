#include "stdafx.h"
#include "PyCollisionShape.h"
#include "Physics/CollisionShape.h"

using namespace boost::python;
using namespace slon::physics;

void exportCollisionShape()
{
    class_< CollisionShape, boost::intrusive_ptr<CollisionShape>, boost::noncopyable >("CollisionShape", no_init);

    class_< SphereShape, boost::intrusive_ptr<SphereShape> >("SphereShape")
        .def_readwrite("radius", &SphereShape::radius);
    implicitly_convertible< boost::intrusive_ptr<SphereShape>, boost::intrusive_ptr<CollisionShape> >();

    class_< BoxShape, boost::intrusive_ptr<BoxShape> >("BoxShape")
        .def_readwrite("halfExtent", &BoxShape::halfExtent);
    implicitly_convertible< boost::intrusive_ptr<BoxShape>, boost::intrusive_ptr<CollisionShape> >();

    class_< ConeShape, boost::intrusive_ptr<ConeShape> >("ConeShape")
        .def_readwrite("radius", &ConeShape::radius)
        .def_readwrite("height", &ConeShape::height);
    implicitly_convertible< boost::intrusive_ptr<ConeShape>, boost::intrusive_ptr<CollisionShape> >();
}