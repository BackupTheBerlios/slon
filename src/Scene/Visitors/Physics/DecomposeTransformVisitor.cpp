#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Scene/Physics/RigidBodyTransform.h"
#include "Scene/Visitors/Physics/DecomposeTransformVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {

    using namespace slon;
    using namespace slon::physics;
    using namespace math;

    void applyRigidBodyTransform( RigidBody&            rigidBody,
                                  const math::Matrix4f& transform,
                                  const math::Vector3f& scaling )
    {
        const CollisionShape* collisionShape = rigidBody.getCollisionShape();
        if ( transform != math::make_identity<float, 4>() && collisionShape )
        {
            CollisionShape* newShape = collisionShape->clone();
            newShape->applyScaling(math::Vector3r(scaling));

            // reset rigid body
            RigidBody::state_desc rigidBodyDesc = rigidBody.getStateDesc();
            rigidBodyDesc.initialTransform = rigidBody.getTransform() * math::Matrix4r(transform);
            rigidBodyDesc.collisionShape.reset(newShape);
            rigidBody.reset(rigidBodyDesc);

            // apply scaling to the constraints
            for ( RigidBody::constraint_iterator iter = rigidBody.firstConstraint();
                                                 iter != rigidBody.endConstraint();
                                                 ++iter )
            {
                Constraint::state_desc constraintDesc = (*iter)->getStateDesc();

                // transform constraint according to scaling
                if ( constraintDesc.rigidBodies[0] == &rigidBody )
                {
                    constraintDesc.frames[0][0][3] *= scaling.x;
                    constraintDesc.frames[0][1][3] *= scaling.y;
                    constraintDesc.frames[0][2][3] *= scaling.z;
                }
                else if ( constraintDesc.rigidBodies[1] == &rigidBody )
                {
                    constraintDesc.frames[1][0][3] *= scaling.x;
                    constraintDesc.frames[1][1][3] *= scaling.y;
                    constraintDesc.frames[1][2][3] *= scaling.z;
                }
                else {
                    assert(!"can't get here");
                }

                (*iter)->reset(constraintDesc);
            }
        }
    }

} // namespace math

namespace slon {
namespace scene {

void DecomposeTransformVisitor::visitAbsoluteTransform(Transform& transform)
{
    RigidBodyTransform* rbTransform = dynamic_cast<RigidBodyTransform*>(&transform);
    if (rbTransform)
    {
        RigidBody* rigidBody = rbTransform->getRigidBody();
        if (rigidBody)
        {
            math::Matrix4f transform = getLocalToWorldTransform();
            math::Vector3f scaling   = math::get_scaling(transform);

            transform *= math::make_scaling(1.0f / scaling.x, 1.0f / scaling.y, 1.0f / scaling.z);

            rbTransform->setScaling(scaling);
            applyRigidBodyTransform(*rigidBody, transform, scaling);
        }
    }

    // Let traverse visitor to do rest of the work
    TraverseVisitor::visitAbsoluteTransform(transform);
}

} // namespace scene
} // namespace slon
