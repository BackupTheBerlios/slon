#include "stdafx.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include "Graphics/Renderable/Debug/DebugDrawSkinned.h"
#include "Scene/Skeleton.h"
#include "Scene/TransformVisitor.h"

namespace slon {
namespace graphics {
namespace debug {

DebugMesh& operator << (DebugMesh& mesh, const joint& j)
{
    float radius = j.radius;

    const scene::Joint* parentJoint = dynamic_cast<const scene::Joint*>(j.debugJoint->getParent());
    if (parentJoint)
    {
        // work in parent joint space
        math::Vector3f tip  = math::get_translation( j.debugJoint->getTransform() );
        math::Vector3f pTip = math::Vector3f(0.0f, 0.0f, 0.0f);

        // connect joint with parent
        mesh << line(pTip, tip);
        radius = std::min(radius, radius * math::length(tip) * j.radiusFromBoneLength);
    }

    // draw joint
    mesh << transform( mesh.transform * j.debugJoint->getTransform() )
         << math::AABBf(-radius, -radius, -radius, radius, radius, radius);

    if (j.drawChildren)
    {
        math::Matrix4f currentTransform = mesh.transform;
        for (const scene::Node* i = j.debugJoint->getChild(); i; i = i->getRight())
        {
            const scene::Joint* childJoint = dynamic_cast<const scene::Joint*>(i);
            if (childJoint) {
                mesh << transform(currentTransform) << joint(childJoint, j.radius, j.radiusFromBoneLength, j.drawChildren);
            }
        }
    }

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const skeleton& s)
{
    const scene::Joint* rootJoint = s.debugSkeleton->getRootJoint();
    if (rootJoint) {
        mesh << joint(rootJoint, s.radius, s.radiusFromBoneLength, true);
    }

    return mesh;
}

} // namespace debug
} // namespace graphics
} // namespace slon
