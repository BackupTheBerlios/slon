#include "stdafx.h"
#include "Scene/Physics/RigidBodyTransform.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {
    math::Matrix4f identityMatrix = math::make_identity<float, 4>();
} // anonymous namespace

namespace slon {
namespace scene {

RigidBodyTransform::RigidBodyTransform(physics::RigidBody* rigidBody_) :
    rigidBody(rigidBody_),
    baseTransform(identityMatrix),
    modifiedCount(0)
{
}

const math::Matrix4f& RigidBodyTransform::getTransform() const
{
    return identityMatrix;
}

const math::Matrix4f& RigidBodyTransform::getInverseTransform() const
{
    return identityMatrix;
}

void RigidBodyTransform::setRigidBody(physics::RigidBody* rigidBody_)
{
    rigidBody.reset(rigidBody_);
    if (rigidBody) {
        localToWorld = baseTransform * math::Matrix4f(rigidBody->getTransform());
    }
}

void RigidBodyTransform::accept(TraverseVisitor& visitor)
{
    if (rigidBody)
    {
        using physics::RigidBody;

        switch ( rigidBody->getDynamicsType() )
        {
        case RigidBody::DT_STATIC:
        case RigidBody::DT_DYNAMIC:
            localToWorld = math::Matrix4f(rigidBody->getTransform()) * baseTransform;
            break;

        case RigidBody::DT_KINEMATIC:
            localToWorld = visitor.getLocalToWorldTransform();
            rigidBody->setTransform( math::Matrix4r(localToWorld * baseTransform) );
            break;

        default:
            assert(!"Can't get here");
        }
    }

    visitor.visitAbsoluteTransform(*this);
}

} // namespace scene
} // namespace slon
