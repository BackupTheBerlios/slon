#include "stdafx.h"
#include "PyVector.h"
#include "PyMatrix.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <sgl/Math/Containers.hpp>
#include <sgl/Math/Matrix.hpp>

using namespace boost::python;
using namespace math;

typedef Matrix<float, 1, 4> VectorRow4f;
typedef Matrix<float, 1, 3> VectorRow3f;

template<typename Vec, typename T, int m>
struct vector_item
{
    typedef Vec                                 matrix_type;
    typedef typename matrix_type::value_type    value_type;

    static value_type& get(matrix_type& mat, int i)
    {
        if (i < 0) {
            i += m;
        }

        if (i >= 0 && i < m) {
            return mat[i];
        }
        else
        {
            IndexError(i, 0, m-1);
            return mat[0];
        }
    }

    static void set(matrix_type& mat, int i, const value_type& r)
    {
        if (i < 0) {
            i += m;
        }

        if (i >= 0 && i < m) {
            mat[i] = r;
        }
        else {
            IndexError(i, 0, m-1);
        }
    }
};

void exportVector()
{
    class_<Vector3f, boost::shared_ptr<Vector3f> >("Vector3f", init<>())
        .def(init<float>())
        .def(init<float, float, float>())
        .def(init<const Vector3f&>())
        .def_readwrite("x", &Vector3f::x)
        .def_readwrite("y", &Vector3f::y)
        .def_readwrite("z", &Vector3f::z)
        .def("__getitem__", &vector_item<Vector3f, float, 3>::get, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &vector_item<Vector3f, float, 3>::set)
        .def(self += self)
        .def(self +  self)
        .def(self -= self)
        .def(self -  self)
        .def(self *= float())
        .def(self *  float())
        .def(self /= float())
        .def(self /  float());

    class_<VectorRow3f, boost::shared_ptr<VectorRow3f> >("VectorRow3f", init<>())
        .def(init<float>())
        .def(init<float, float, float>())
        .def(init<const VectorRow3f&>())
        .def_readwrite("x", &VectorRow3f::x)
        .def_readwrite("y", &VectorRow3f::y)
        .def_readwrite("z", &VectorRow3f::z)
        .def("__getitem__", &vector_item<VectorRow3f, float, 3>::get, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &vector_item<VectorRow3f, float, 3>::set)
        .def(self += self)
        .def(self +  self)
        .def(self -= self)
        .def(self -  self)
        .def(self *= float())
        .def(self *  float())
        .def(self /= float())
        .def(self /  float());

    class_<Vector4f, boost::shared_ptr<Vector4f>, boost::noncopyable>("Vector4f", init<>())
        .def(init<float>())
        .def(init<float, float, float, float>())
        .def(init<const Vector4f&>())
        .def_readwrite("x", &Vector4f::x)
        .def_readwrite("y", &Vector4f::y)
        .def_readwrite("z", &Vector4f::z)
        .def_readwrite("w", &Vector4f::w)
        .def("__getitem__", &vector_item<Vector4f, float, 4>::get, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &vector_item<Vector4f, float, 4>::set)
        .def(self += self)
        .def(self +  self)
        .def(self -= self)
        .def(self -  self)
        .def(self *= float())
        .def(self *  float())
        .def(self /= float())
        .def(self /  float());

    class_<VectorRow4f, boost::shared_ptr<VectorRow4f>, boost::noncopyable>("VectorRow4f", init<>())
        .def(init<float>())
        .def(init<float, float, float, float>())
        .def(init<const VectorRow4f&>())
        .def_readwrite("x", &VectorRow4f::x)
        .def_readwrite("y", &VectorRow4f::y)
        .def_readwrite("z", &VectorRow4f::z)
        .def_readwrite("w", &VectorRow4f::w)
        .def("__getitem__", &vector_item<VectorRow4f, float, 4>::get, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &vector_item<VectorRow4f, float, 4>::set)
        .def(self += self)
        .def(self +  self)
        .def(self -= self)
        .def(self -  self)
        .def(self *= float())
        .def(self *  float())
        .def(self /= float())
        .def(self /  float());
}