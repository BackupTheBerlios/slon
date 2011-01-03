#ifndef __SLON_ENGINE_PHYSICS_VISITORS_PHYSICS_SKELETON_BUILDER_VISITOR_H__
#define __SLON_ENGINE_PHYSICS_VISITORS_PHYSICS_SKELETON_BUILDER_VISITOR_H__

#include "../../Physics/PhysicsSkeleton.h"
#include "../NodeVisitor.h"
#include <stack>

namespace slon {
namespace scene {

/** Builds physics skeleton from the subgraph. */
class PhysicsSkeletonBuilderVisitor :
    public NodeVisitor
{
public:
    typedef NodeVisitor                     base_type;
    typedef PhysicsSkeletonBuilderVisitor   this_type;

    typedef std::stack<PhysicsBone*>        physics_bone_stack;

public:
    PhysicsSkeletonBuilderVisitor(RigidBodyTransform& baseTransform);

    // override NodeVisitor
    void visitNode(scene::Node& node);
    void visitGroup(scene::Group& group);
    void visitTransform(scene::Transform& transform);

    virtual ~PhysicsSkeletonBuilderVisitor() {}

private:
    PhysicsBone* getCurrentBaseBone();

protected:
    rigid_body_transform_ptr baseTransform;
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_VISITORS_PHYSICS_SKELETON_BUILDER_VISITOR_H__
