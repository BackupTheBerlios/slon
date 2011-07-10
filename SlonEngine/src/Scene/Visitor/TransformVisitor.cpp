#include "stdafx.h"
#include "Scene/Entity.h"
#include "Scene/Skeleton.h"
#include "Scene/Visitor/TransformVisitor.h"
#include "Utility/math.hpp"

using namespace slon;
using namespace scene;
using namespace math;

namespace {
    math::Matrix4f identityMatrix = math::make_identity<float, 4>();
} // anonymous namespace

TransformVisitor::TransformVisitor()
:   currentNode(0)
{
}

TransformVisitor::TransformVisitor(Node& node)
:   currentNode(0)
{
    traverse(node);
}

void TransformVisitor::traverse(Node& node)
{
    // initialize
    aabb = bounds<math::AABBf>::inv_infinite();

    // traverse
    forTraverse.push( traverse_node(&node) );
    while ( !forTraverse.empty() )
    {
        traverse_node tn = forTraverse.top(); forTraverse.pop();
        currentNode = &tn;

		Node::TYPE type = tn.node->getNodeType();
        if (type & Node::JOINT_BIT) {
            visitJoint( static_cast<Joint&>(*tn.node) );
        }
        else if (type & Node::TRANSFORM_BIT) {
            visitTransform( static_cast<Transform&>(*tn.node) );
        }
        else if (type & Node::GROUP_BIT) {
            visitGroup( static_cast<Group&>(*tn.node) );
        }
        else if (type & Node::SKELETON_BIT) {
            visitSkeleton( static_cast<Skeleton&>(*tn.node) );
        }
        else if (type & Node::ENTITY_BIT) {
            visitEntity( static_cast<Entity&>(*tn.node) );
        }
    }
    currentNode = 0;

    assert( forTraverse.empty() );    
}

void TransformVisitor::visitGroup(Group& group)
{
    traverse_node child = *currentNode;
    for(Node* i = group.getChild(); i; i = i->getRight())
    {
        child.node = i;
        forTraverse.push(child);
    }
}

bool TransformVisitor::visitTransform(Transform& transform)
{
    bool dirty = currentNode->dirty | (transform.getTransformTraverseStamp() < transform.getModifiedCount());
    if (!transform.isAbsolute() && dirty) 
    {
        if (currentNode->transform)
        {
            // if transform have parent, get transformation from it
            transform.localToWorld = currentNode->transform->getLocalToWorld() * transform.getTransform();
            transform.worldToLocal = transform.getInverseTransform() * currentNode->transform->getWorldToLocal();
        }
        else
        {
            transform.localToWorld = transform.getTransform();
            transform.worldToLocal = transform.getInverseTransform();
        }
    }
    transform.traverseStamp = transform.getModifiedCount();

    traverse_node child;
    child.transform = &transform;
    child.dirty     = dirty;
    for(Node* i = transform.getChild(); i; i = i->getRight())
    {
        child.node = i;
        forTraverse.push(child);
    }

    return dirty;
}

void TransformVisitor::visitEntity(Entity& entity)
{
    aabb = math::merge( aabb, getLocalToWorldTransform() * entity.getBounds() );
    entity.accept(*this);
}

void TransformVisitor::visitSkeleton(Skeleton& skeleton)
{
    if ( Joint* root = skeleton.getRootJoint() ) {
        TransformVisitor tv(*root); // Run new traverse visitor to compute transforms in skeleton local space
    }
}

void TransformVisitor::visitJoint(Joint& joint)
{
    if ( visitTransform(joint) ) {
        joint.skinningMatrix = joint.localToWorld * joint.inverseBindMatrix;
    }
}

const math::Matrix4f& TransformVisitor::getLocalToWorldTransform() const 
{
    if (currentNode && currentNode->transform) {
        return currentNode->transform->localToWorld;
    }
    else if (!forTraverse.empty() && forTraverse.top().transform) {
        return forTraverse.top().transform->localToWorld;
    }
    return identityMatrix;
}

const math::Matrix4f& TransformVisitor::getWorldToLocalTransform() const 
{
    if (currentNode && currentNode->transform) {
        return currentNode->transform->localToWorld;
    }
    else if (!forTraverse.empty() && forTraverse.top().transform) {
        return forTraverse.top().transform->worldToLocal;
    }
    return identityMatrix;
}
