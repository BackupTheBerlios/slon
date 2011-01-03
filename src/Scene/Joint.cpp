#include "stdafx.h"
#include "Scene/Joint.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace slon {
namespace scene {

Joint::Joint() :
    inverseBindMatrix( math::make_identity<float, 4>() ),
    id(0)
{
}

// Override node
void Joint::accept(TraverseVisitor& visitor) 
{ 
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitJoint(*this);
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
