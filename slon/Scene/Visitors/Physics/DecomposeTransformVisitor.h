#ifndef __SLON_ENGINE_SCENE_PHYSICS_DECOMPOSE_TRANSFORM_VISITOR__
#define __SLON_ENGINE_SCENE_PHYSICS_DECOMPOSE_TRANSFORM_VISITOR__

#include "../TraverseVisitor.h"

namespace slon {
namespace scene {

/** Visitor setups initial rigid body transform, retrieved
 * from the scene graph, removes scaling from that transform
 * and adds it as post transform to the RigidBodyTransform.
 */
class DecomposeTransformVisitor :
    public TraverseVisitor
{
public:
    // Override TraverseVisitor for RigidBodyTransform
    virtual void visitAbsoluteTransform(Transform& transform);
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_PHYSICS_DECOMPOSE_TRANSFORM_VISITOR__
