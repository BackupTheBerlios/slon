#include "stdafx.h"
#include "PyAABB.h"
#include <sgl/Math/AABB.hpp>

using namespace boost::python;
using namespace math;

void exportAABB()
{
    class_<AABB3f, boost::shared_ptr<AABB3f>, boost::noncopyable>("AABB3f")
        .def(init<>())
        .def(init<const Vector3f&, const Vector3f&>())
        .def(init<float, float, float, float, float, float>())
        .def("extend",      &AABB3f::extend,    return_value_policy<reference_existing_object>())
        .def("reset_min",   &AABB3f::reset_min, return_value_policy<reference_existing_object>())
        .def("reset_max",   &AABB3f::reset_max, return_value_policy<reference_existing_object>())
        .def("size",        &AABB3f::size,      return_value_policy<return_by_value>())
        .def("center",      &AABB3f::center,    return_value_policy<return_by_value>());
}