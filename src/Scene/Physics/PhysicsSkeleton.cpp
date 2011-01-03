#include "Scene/Physics/PhysicsSkeleton.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {

} // anonymous namespace

namespace slon {
namespace scene {

// Override bone
const math::Quaternionf& PhysicsBone::getRotation() const
{
    return rotation;
}

const math::Vector3f& PhysicsBone::getTranslation() const
{
    return translation;
}

void PhysicsBone::applyTorque(const math::Vector3f& torque)
{
    assert(rigidBody);

    float r = length(boneEnd - boneStart) + math::EPS_3f;
    rigidBody->applyForce(boneEnd, torque / r);
}

// Override Transform
math::Matrix4f& PhysicsBone::getTransformMatrix(math::Matrix4f& out) const
{
    return out = localTransform;
}

math::Matrix4f& PhysicsBone::getInverseTransformMatrix(math::Matrix4f& out) const
{
    return out = math::invert(localTransform);
}

void PhysicsBone::accept(scene::TraverseVisitor& visitor)
{
    if (rigidBody)
    {
        // retrieve bone transforms
        localToWorld   = rigidBody->getTransform();
        localTransform = localToWorld * visitor.getWorldToLocalTransform();

        //FIXME
        //translation = math::get_translation(localTransform)
        //            + math::get_translation(localTransform * boneEnd->get_desc().);
        //rotation    = math::from_matrix(localTransform);

        // check scaling
        // assert( math::length( math::get_scaling(localTransform) ) < 1.0f + math::EPS_3f );
    }

    visitor.visitAbsoluteTransform(*this);
}

} // namespace physics
} // namespace slon
