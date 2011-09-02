#include "stdafx.h"
#include "PyCollisionShape.h"
#include "Physics/CollisionShape.h"

using namespace boost::python;
using namespace slon::physics;

void exportCollisionShape()
{
    class_< CollisionShape, boost::intrusive_ptr<CollisionShape>, boost::noncopyable >("CollisionShape", no_init);

    class_< SphereShape, bases<CollisionShape>, boost::intrusive_ptr<SphereShape> >("SphereShape")
        .def(init<>())
        .def(init<float>())
        .def_readwrite("radius", &SphereShape::radius);
    implicitly_convertible< boost::intrusive_ptr<SphereShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<SphereShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const SphereShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< BoxShape, bases<CollisionShape>, boost::intrusive_ptr<BoxShape> >("BoxShape")
        .def(init<>())
        .def(init<float, float, float>())
        .def(init<math::Vector3f>())
        .def_readwrite("halfExtent", &BoxShape::halfExtent);
    implicitly_convertible< boost::intrusive_ptr<BoxShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<BoxShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const BoxShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< ConeShape, bases<CollisionShape>, boost::intrusive_ptr<ConeShape> >("ConeShape")
        .def(init<>())
        .def(init<float, float>())
        .def_readwrite("radius", &ConeShape::radius)
        .def_readwrite("height", &ConeShape::height);
    implicitly_convertible< boost::intrusive_ptr<ConeShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<ConeShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const ConeShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< CylinderXShape, bases<CollisionShape>, boost::intrusive_ptr<CylinderXShape> >("CylinderXShape")
        .def(init<>())
        .def(init<float, float, float>())
        .def(init<math::Vector3f>())
        .def_readwrite("halfExtent", &CylinderXShape::halfExtent);
    implicitly_convertible< boost::intrusive_ptr<CylinderXShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<CylinderXShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const CylinderXShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< CylinderYShape, bases<CollisionShape>, boost::intrusive_ptr<CylinderYShape> >("CylinderYShape")
        .def(init<>())
        .def(init<float, float, float>())
        .def(init<math::Vector3f>())
        .def_readwrite("halfExtent", &CylinderYShape::halfExtent);
    implicitly_convertible< boost::intrusive_ptr<CylinderYShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<CylinderYShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const CylinderYShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< CylinderZShape, bases<CollisionShape>, boost::intrusive_ptr<CylinderZShape> >("CylinderZShape")
        .def(init<>())
        .def(init<float, float, float>())
        .def(init<math::Vector3f>())
        .def_readwrite("halfExtent", &CylinderZShape::halfExtent);
    implicitly_convertible< boost::intrusive_ptr<CylinderZShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<CylinderZShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const CylinderZShape>, boost::intrusive_ptr<const CollisionShape> >();

    class_< ConvexShape, bases<CollisionShape>, boost::intrusive_ptr<ConvexShape> >("ConvexShape")
        .def(init<>())
        .def_readwrite("vertices",  &ConvexShape::vertices)
        .def("buildConvexHull",     &ConvexShape::buildConvexHull);
    implicitly_convertible< boost::intrusive_ptr<ConvexShape>,       boost::intrusive_ptr<CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<ConvexShape>,       boost::intrusive_ptr<const CollisionShape> >();
    implicitly_convertible< boost::intrusive_ptr<const ConvexShape>, boost::intrusive_ptr<const CollisionShape> >();
}