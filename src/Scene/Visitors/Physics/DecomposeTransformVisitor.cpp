#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Scene/Physics/RigidBodyTransform.h"
#include "Scene/Visitors/Physics/DecomposeTransformVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace slon {
namespace scene {

void DecomposeTransformVisitor::visitAbsoluteTransform(Transform& transform)
{
    RigidBodyTransform* rbTransform = dynamic_cast<RigidBodyTransform*>(&transform);
    if (rbTransform)
    {
        physics::RigidBody* rigidBody = rbTransform->getRigidBody();
        if (rigidBody)
        {
            switch ( rigidBody->getDynamicsType() )
            {
                case physics::RigidBody::DT_STATIC:
                case physics::RigidBody::DT_DYNAMIC:
                {             
                    math::Matrix4f T = getLocalToWorldTransform();
                    math::Matrix4f R = math::Matrix4f(rigidBody->getTransform());
                    rbTransform->setBaseTransform( math::invert(R) * T );
                    break;
                }

                case physics::RigidBody::DT_KINEMATIC:
                {
                    math::Matrix4f T = getLocalToWorldTransform();
                    math::Matrix4f R = math::Matrix4f(rigidBody->getTransform());
                    rbTransform->setBaseTransform( math::invert(T) * R );
                    break;
                }
            }
        }
    }

    // Let traverse visitor to do rest of the work
    TraverseVisitor::visitAbsoluteTransform(transform);
}

} // namespace scene
} // namespace slon
