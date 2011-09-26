#include "stdafx.h"
//#include "Log/LogVisitor.h"
#include "Scene/Entity.h"
#include "Scene/Skeleton.h"
#include "Scene/TransformVisitor.h"
#include "Utility/math.hpp"

using namespace slon;
using namespace scene;
using namespace math;

namespace {
    math::Matrix4f identityMatrix = math::Matrix4f::identity();
}

//DECLARE_AUTO_LOGGER("scene.TransformVisitor");

TransformVisitor::TransformVisitor()
:   currentTransform(0)
{
}

TransformVisitor::TransformVisitor(Node& node)
:   currentTransform(0)
{
    traverse(node);
}

void TransformVisitor::traverse(Node& node)
{
    //AUTO_LOGGER_INIT;
    //log::LogVisitor vis(AUTO_LOGGER, log::S_FLOOD, node);

    // initialize
    aabb = bounds<math::AABBf>::inv_infinite();
/*
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
*/
    Node* currentNode = &node;
    currentTransform  = 0;
    while (currentNode)
    {
        Transform* visitedTransform = 0;
        Group*     currentGroup     = 0;
		Node::TYPE type             = currentNode->getNodeType();
        if (type & Node::JOINT_BIT)
        {
            visitJoint( currentTransform, static_cast<Joint&>(*currentNode) );
            currentGroup = visitedTransform = static_cast<Transform*>(currentNode);
        }
        else if (type & Node::TRANSFORM_BIT)
        {
            currentGroup = visitedTransform = static_cast<Transform*>(currentNode);
            visitTransform(currentTransform, *visitedTransform);
        }
        else if (type & Node::GROUP_BIT) {
            currentGroup = static_cast<Group*>(currentNode);
        }
        else if (type & Node::SKELETON_BIT) {
            visitSkeleton( static_cast<Skeleton&>(*currentNode) );
        }
        else if (type & Node::ENTITY_BIT) {
            visitEntity( currentTransform, static_cast<Entity&>(*currentNode) );
        }

        if ( currentGroup && currentGroup->getChild() ) // go down
        {
            currentNode = currentGroup->getChild();
            if (visitedTransform) {
                currentTransform = visitedTransform;
            }
        }
        else if ( currentNode->getRight() ) // go right
        {
            currentNode = currentNode->getRight();
        }
        else // go up
        {
            while ( currentNode != &node && !currentNode->getRight() )
            {
                currentNode = currentNode->getParent();
                if (currentNode == currentTransform) {
                    currentTransform = 0;
                }
            }

            if (currentNode == &node) {
                return;
            }
            else 
            {
                currentNode = currentNode->getRight();
                
                // search for parent transform
                if (!currentTransform)
                {
                    Node* transformNode = currentNode->getParent();
                    while (transformNode)
                    {
                        if (transformNode->getNodeType() & Node::TRANSFORM_BIT)
                        {
                            currentTransform = static_cast<Transform*>(transformNode);
                            break;
                        }
                        transformNode = transformNode->getParent();
                    }
                }
            }
        }
    }

    currentTransform = 0;
}

bool TransformVisitor::visitTransform(Transform* parentTransform, Transform& transform)
{
    // means dirty
    transform.updated = (parentTransform && parentTransform->updated) 
                        || (transform.getTransformTraverseStamp() < transform.getModifiedCount());
    if (transform.updated) 
    {
        if (transform.isAbsolute() || !parentTransform)
        {
            transform.localToWorld = transform.getTransform();
            transform.worldToLocal = transform.getInverseTransform();
        }
        else if (parentTransform)
        {
            transform.localToWorld = parentTransform->getLocalToWorld() * transform.getTransform();
            transform.worldToLocal = transform.getInverseTransform() * parentTransform->getWorldToLocal();
        }
    }
    transform.traverseStamp = transform.getModifiedCount();

    return transform.updated;
}

void TransformVisitor::visitEntity(Transform* parentTransform, Entity& entity)
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

void TransformVisitor::visitJoint(Transform* parentTransform, Joint& joint)
{
    if ( visitTransform(parentTransform, joint) ) {
        joint.skinningMatrix = joint.localToWorld * joint.inverseBindMatrix;
    }
}

const math::Matrix4f& TransformVisitor::getLocalToWorldTransform() const 
{
    if (currentTransform) {
        return currentTransform->localToWorld;
    }
    return identityMatrix;
}

const math::Matrix4f& TransformVisitor::getWorldToLocalTransform() const 
{
    if (currentTransform) {
        return currentTransform->localToWorld;
    }
    return identityMatrix;
}
