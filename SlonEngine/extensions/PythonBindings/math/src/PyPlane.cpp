#include "stdafx.h"
#include "PyPlane.h"
#include <sgl/Math/Plane.hpp>

using namespace boost::python;
using namespace math;

void exportPlane()
{
    class_<Planef, boost::shared_ptr<Planef>, boost::noncopyable>("Planef")
        .def(init<>())
        .def(init<const Vector3f&, float>())
        .def(init<float, float, float, float>())
        .def(init<const Vector3f&, const Vector3f&, const Vector3f&>())
        .def_readwrite("normal",   &Planef::normal)
        .def_readwrite("distance", &Planef::distance);
}