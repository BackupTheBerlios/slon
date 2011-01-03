#include "stdafx.h"
#include "Scene/Physics/RigidBodyTransform.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {
    
    void scale_axises(math::Matrix4f& transform, const math::Vector3f& scaling)
    {
        transform[0][0] *= scaling.x;
        transform[0][1] *= scaling.x;
        transform[0][2] *= scaling.x;
        transform[0][3] *= scaling.x;

        transform[1][0] *= scaling.y;
        transform[1][1] *= scaling.y;
        transform[1][2] *= scaling.y;
        transform[1][3] *= scaling.y;

        transform[2][0] *= scaling.z;
        transform[2][1] *= scaling.z;
        transform[2][2] *= scaling.z;
        transform[2][3] *= scaling.z;
    }

    math::Matrix4f identityMatrix = math::make_identity<float, 4>();

} // anonymous namespace

namespace slon {
namespace scene {

RigidBodyTransform::RigidBodyTransform(physics::RigidBody* rigidBody_) :
    rigidBody(rigidBody_),
    scaling(1.0f, 1.0f, 1.0f),
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
    if (rigidBody) 
    {
        localToWorld = rigidBody->getTransform();
        scale_axises(localToWorld, scaling);
    }
}

void RigidBodyTransform::accept(TraverseVisitor& visitor)
{
    if (rigidBody)
    {
        using physics::RigidBody;

        switch (rigidBody->getDynamicsType() )
        {
        case RigidBody::DT_STATIC:
        case RigidBody::DT_DYNAMIC:
            localToWorld = rigidBody->getTransform();
            //scale_axises(localToWorld, scaling);
            localToWorld *= math::make_scaling(scaling.x, scaling.y, scaling.z);
            break;

        case RigidBody::DT_KINEMATIC:
            localToWorld = visitor.getLocalToWorldTransform();
            rigidBody->setTransform(localToWorld);
            localToWorld *= math::make_scaling(scaling.x, scaling.y, scaling.z);
            //scale_axises(localToWorld, scaling);
            break;

        default:
            assert(!"Can't get here");
        }
    }

    visitor.visitAbsoluteTransform(*this);
}

} // namespace scene
} // namespace slon
