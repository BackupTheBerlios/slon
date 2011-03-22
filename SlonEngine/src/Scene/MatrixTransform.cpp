#include "stdafx.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/MatrixTransform.h"
#include <sgl/Math/MatrixFunctions.hpp>

using namespace slon;
using namespace scene;
using namespace math;

MatrixTransform::MatrixTransform() :
    transformDirty(false),
    invTransformDirty(false)
{
	localToWorld = transform    = math::make_identity<float, 4>();
    worldToLocal = invTransform = math::make_identity<float, 4>();
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
    update();
}

void MatrixTransform::setInverseTransform(const Matrix4f& matrix)
{
    invTransform   = matrix;
    transformDirty = true;
    update();
}

void MatrixTransform::setTransformAndInverse(const math::Matrix4f& matrix, const math::Matrix4f& invMatrix)
{
    transform      = matrix;
    invTransform   = invMatrix;
    transformDirty = invTransformDirty = false;
    update();
}    

void MatrixTransform::accept(log::LogVisitor& visitor) const
{
    visitor << "MatrixTransform";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "transform =" << log::detailed(getTransform(), true)  
            << "localToWorld =" << log::detailed(getLocalToWorld(), true);
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}