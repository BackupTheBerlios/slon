#include "stdafx.h"
#include "PyContainers.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <sgl/Math/Containers.hpp>

using namespace boost::python;

void exportContainers()
{
    class_<std::vector<short> >("ShortArray")
        .def(vector_indexing_suite<std::vector<short> >());

    class_<std::vector<unsigned short> >("UShortArray")
        .def(vector_indexing_suite<std::vector<unsigned short> >());

    class_<std::vector<int> >("IntArray")
        .def(vector_indexing_suite<std::vector<int> >());

    class_<std::vector<unsigned int> >("UIntArray")
        .def(vector_indexing_suite<std::vector<unsigned int> >());

    class_<std::vector<float> >("FloatArray")
        .def(vector_indexing_suite<std::vector<float> >());

    class_<std::vector<double> >("DoubleArray")
        .def(vector_indexing_suite<std::vector<double> >());

    // vector containers
    class_<math::vector_of_vector2f>("Vector2fArray")
        .def(vector_indexing_suite<math::vector_of_vector2f>());
    class_<math::vector_of_vector3f>("Vector3fArray")
        .def(vector_indexing_suite<math::vector_of_vector3f>());
    //class_<math::vector_of_vector4f>("Vector4fArray")
    //    .def(vector_indexing_suite<math::vector_of_vector4f, true>());

    class_<math::vector_of_vector2d>("Vector2dArray")
        .def(vector_indexing_suite<math::vector_of_vector2d>());
    class_<math::vector_of_vector3d>("Vector3dArray")
        .def(vector_indexing_suite<math::vector_of_vector3d>());
    class_<math::vector_of_vector4d>("Vector4dArray")
        .def(vector_indexing_suite<math::vector_of_vector4d>());

    class_<math::vector_of_vector2i>("Vector2iArray")
        .def(vector_indexing_suite<math::vector_of_vector2i>());
    class_<math::vector_of_vector3i>("Vector3iArray")
        .def(vector_indexing_suite<math::vector_of_vector3i>());
    class_<math::vector_of_vector4i>("Vector4iArray")
        .def(vector_indexing_suite<math::vector_of_vector4i>());
}