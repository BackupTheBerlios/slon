#include "stdafx.h"
#include "PyMatrixTransform.h"
#include "Scene/MatrixTransform.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

// wrappers
matrix_transform_ptr MatrixTransformCreate1(const std::string& name)
{
    return matrix_transform_ptr( new MatrixTransform(name) );
}

matrix_transform_ptr MatrixTransformCreate2(const math::Matrix4f& matrix)
{
    return matrix_transform_ptr( new MatrixTransform(matrix) );
}

matrix_transform_ptr MatrixTransformCreate3(const std::string& name, const math::Matrix4f& matrix)
{
    return matrix_transform_ptr( new MatrixTransform(name, matrix) );
}

void exportMatrixTransform()
{
    class_<MatrixTransform, bases<Group>, boost::intrusive_ptr<MatrixTransform>, boost::noncopyable>("MatrixTransform", init<>())
        .def("__init__", make_constructor(MatrixTransformCreate1))
        .def("__init__", make_constructor(MatrixTransformCreate2))
        .def("__init__", make_constructor(MatrixTransformCreate3));
}