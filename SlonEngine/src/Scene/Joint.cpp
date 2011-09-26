#include "stdafx.h"
#include "Scene/Joint.h"
#include <sgl/Math/Quaternion.hpp>

namespace slon {
namespace scene {

Joint::Joint() :
    id(0)
{
    inverseBindMatrix.make_identity();
}

// Override transform
void Joint::setTransform(const math::Quaternionf& rotation, const math::Vector3f& translation)
{
    transform         = math::to_matrix_4x4(rotation, translation);
    invTransformDirty = true;
    ++modifiedCount;
}

void Joint::setInverseTransform(const math::Quaternionf& rotation, const math::Vector3f& translation)
{
    invTransform   = math::to_matrix_4x4(rotation, translation);
    transformDirty = true;
    ++modifiedCount;
}

} // namespace scene
} // namespace slon
