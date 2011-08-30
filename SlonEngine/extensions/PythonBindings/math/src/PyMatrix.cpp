#include "stdafx.h"
#include "PyMatrix.h"
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <sgl/Math/Matrix.hpp>

using namespace boost::python;
using namespace math;

// wrappers
Matrix4f make_matrix4f( float _0,  float _1,  float _2,  float _3,
                        float _4,  float _5,  float _6,  float _7,
                        float _8,  float _9,  float _10, float _11,
                        float _12, float _13, float _14, float _15 )
{
    return make_matrix(_0,  _1,  _2,  _3,
                       _4,  _5,  _6,  _7,
                       _8,  _9,  _10, _11,
                       _12, _13, _14, _15);
}

template<typename T, int n, int m>
struct matrix_item
{
    typedef math::Matrix<T, n, m>           matrix_type;
    typedef typename matrix_type::row_type  row_type;

    static row_type& get(matrix_type& mat, int i)
    {
        if (i < 0) {
            i += n;
        }

        if (i >= 0 && i < n) {
            return mat[i];
        }

        IndexError(i, 0, n-1);
        return mat[0];
    }

    static void set(matrix_type& mat, int i, const row_type& r)
    {
        if (i < 0) {
            i += n;
        }

        if (i >= 0 && i < n) {
            mat[i] = r;
        }

        IndexError(i, 0, n-1);
    }
};

void exportMatrix()
{
    class_<Matrix4f, boost::shared_ptr<Matrix4f>, boost::noncopyable>("Matrix4f", init<>())
        .def(init<float>())
        .def(init<const Matrix4f&>())
        .def("__getitem__", &matrix_item<float, 4, 4>::get, return_value_policy<reference_existing_object>())
        .def("__setitem__", &matrix_item<float, 4, 4>::set)
        .def(self += self)
        .def(self +  self)
        .def(self -= self)
        .def(self -  self)
        .def(self *= float())
        .def(self *  float())
        .def(self *= self)
        .def(self *  self)
        .def(self *  Vector4f())
        .def(self /= float())
        .def(self /  float());
}