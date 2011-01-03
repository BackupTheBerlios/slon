#include "stdafx.h"
#include "Scene/MatrixTransform.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

using namespace slon;
using namespace scene;
using namespace math;

MatrixTransform::MatrixTransform() :
    modifiedCount(0),
    transformDirty(false),
    invTransformDirty(false)
{
	transform    = math::make_identity<float, 4>();
    invTransform = math::make_identity<float, 4>();
}

// Override transform
const math::Matrix4f& MatrixTransform::getTransform() const
{
    if (transformDirty)
    {
        const_cast<MatrixTransform*>(this)->transform = math::invert(invTransform);
        transformDirty = false;
    }

    return transform;
}

const math::Matrix4f& MatrixTransform::getInverseTransform() const
{
    if (invTransformDirty)
    {
        const_cast<MatrixTransform*>(this)->invTransform = math::invert(transform);
        invTransformDirty = false;
    }

    return invTransform;
}

void MatrixTransform::setTransform(const Matrix4f& matrix)
{
    transform         = matrix;
    invTransformDirty = true;
    ++modifiedCount;
}

void MatrixTransform::setInverseTransform(const Matrix4f& matrix)
{
    invTransform   = matrix;
    transformDirty = true;
    ++modifiedCount;
}

void MatrixTransform::setTransformAndInverse(const math::Matrix4f& matrix, const math::Matrix4f& invMatrix)
{
    transform      = matrix;
    invTransform   = invMatrix;
    transformDirty = invTransformDirty = false;
    ++modifiedCount;
}