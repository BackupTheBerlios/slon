#include "stdafx.h"
#include "PyMatrixTransform.h"
#include "Scene/MatrixTransform.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

// wrappers
matrix_transform_ptr MatrixTransformCreate(const std::string& name, const math::Matrix4f& matrix)
{
    return matrix_transform_ptr( new MatrixTransform(name, matrix) );
}

void exportMatrixTransform()
{
    class_<MatrixTransform, bases<Group>, boost::intrusive_ptr<MatrixTransform>, boost::noncopyable>("MatrixTransform", init<>())
        .def("__init__", make_constructor(MatrixTransformCreate));
}