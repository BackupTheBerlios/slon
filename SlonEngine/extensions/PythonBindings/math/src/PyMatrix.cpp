#include "stdafx.h"
#include "PyMatrix.h"
#include <sgl/Math/Matrix.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

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

void exportMatrix()
{
    class_<Matrix4f, boost::shared_ptr<Matrix4f>, boost::noncopyable>("Matrix4f", init<>())
        .def(init<float>())
        .def(init<const Matrix4f&>())
        .def(self += self)
        .def(self + self)
        .def(self -= self)
        .def(self - self)
        .def(self *= float())
        .def(self * float())
        .def(self *= self)
        .def(self * self)
        .def(self * Vector4f())
        .def(self /= float())
        .def(self / float());
}