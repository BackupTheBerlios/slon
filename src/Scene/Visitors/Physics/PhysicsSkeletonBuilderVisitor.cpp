#include "Scene/Visitors/Physics/PhysicsSkeletonBuilderVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {

} // anonymous namespace

namespace slon {
namespace scene {

PhysicsSkeletonBuilderVisitor::PhysicsSkeletonBuilderVisitor(RigidBodyTransform& baseTransform) :
    baseTransform(&_baseTransform)
{
}

/*
PhysicsBone* PhysicsSkeletonBuilderVisitor::getCurrentBaseBone()
{
    if ( physicsBones.empty() )
    {
        // create root bone
        PhysicsBone* rootBone = new PhysicsBone();
        physicsSkeleton->setRootBone(rootBone);
        physicsBones.push(rootBone);
    }

    return physicsBones.top();
}

void PhysicsSkeletonBuilderVisitor::visitNode(scene::Node& node)
{
    // copy node
    PhysicsBone* physicsBone = getCurrentBaseBone();
    physicsBone->addChild(node);
}

void PhysicsSkeletonBuilderVisitor::visitGroup(scene::Group& group)
{
    // copy group
    PhysicsBone* physicsBone = getCurrentBaseBone();
    physicsBone->addChild(group);

    // traverse children
    base_type::visitGroup(group);
}

void PhysicsSkeletonBuilderVisitor::visitTransform(scene::Transform& transform)
{
    // FIXME: calculate bone orientation
    PhysicsBone* physicsBone = new PhysicsBone();

    // add bone to skeleton
    if ( physicsBones.empty() ) {
        physicsSkeleton->setRootBone(physicsBone);
    }
    else
    {
        PhysicsBone* baseBone = getCurrentBaseBone();
        baseBone->addChild(*physicsBone);
    }

    // traverse and restore base bone
    physicsBones.push(physicsBone);
    base_type::visitGroup(transform);
    physicsBones.pop();
}
*/
} // namespace scene
} // namespace slon
