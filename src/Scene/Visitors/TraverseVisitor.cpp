#include "stdafx.h"
#include "Scene/MatrixTransform.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Utility/math.hpp"

using namespace slon;
using namespace scene;
using namespace math;

namespace {
    math::Matrix4f identityMatrix = math::make_identity<float, 4>();
} // anonymous namespace

TraverseVisitor::TraverseVisitor() :
    changeTransforms(false)
{
}

void TraverseVisitor::traverse(Node& node)
{
    aabb             = bounds<math::AABBf>::inv_infinite();
    changeTransforms = false;
    forTraverse.push_back(&node);
    transformDepthStack.push_back(0);
    while ( !forTraverse.empty() )
    {
        Node* traversed = forTraverse.back(); forTraverse.pop_back();
        transformNodeStack.resize( transformDepthStack.back() ); transformDepthStack.pop_back();
        acceptBy(*traversed);
    }
   
    transformNodeStack.clear();
    assert( transformDepthStack.empty() );    
}

void TraverseVisitor::visitTransform(Transform& transform)
{
    if ( (transform.getAcceptMask() & visitMask) == 0 ) {
        return;
    }

    /*
    if (changeTransforms) 
    {
        const Transform* lastTransform = transformNodeStack.back();
        transform.localToWorld         = lastTransform->getLocalToWorld() * transform.getTransform();
        transform.worldToLocal         = transform.getInverseTransform() * lastTransform->getWorldToLocal();
    }
    else if ( transform.getTraverseTS() != transform.getModifiedTS() )
    */
    {
        if ( !transformNodeStack.empty() )
        {
            // if transform have parent, get transformation from it
            const Transform* lastTransform = transformNodeStack.back();
            transform.localToWorld         = lastTransform->getLocalToWorld() * transform.getTransform();
            transform.worldToLocal         = transform.getInverseTransform() * lastTransform->getWorldToLocal();
        }
        else
        {
            transform.localToWorld = transform.getTransform();
            transform.worldToLocal = transform.getInverseTransform();
        }

        changeTransforms = true;
    }

    transformNodeStack.push_back(&transform);
    visitGroup(transform);

    /*
    if ( transform.getTraverseTS() != transform.getModifiedTS() ) {
        changeTransforms = false;
    }
    */

    transform.traverseTS = transform.getModifiedTS();
}

void TraverseVisitor::visitEntity(Entity& entity)
{
    aabb = math::merge( aabb, getLocalToWorldTransform() * entity.getBounds() );
}

void TraverseVisitor::visitAbsoluteTransform(Transform& transform)
{
    if ( (transform.getAcceptMask() & visitMask) == 0 ) {
        return;
    }

    bool oldChangeTransforms = changeTransforms;
    changeTransforms = true;

    transformNodeStack.push_back(&transform);
    visitGroup(transform);

    changeTransforms     = oldChangeTransforms;
    transform.traverseTS = transform.getModifiedTS();
}

void TraverseVisitor::visitSkeleton(Skeleton& skeleton)
{
    if ( (skeleton.getAcceptMask() & visitMask) == 0 ) {
        return;
    }

    if ( Joint* root = skeleton.getRootJoint() )
    {
        // Run new traverse visitor to compute transforms in skeleton local space
        TraverseVisitor tv;
        tv.traverse(*root);
    }
}

void TraverseVisitor::visitJoint(Joint& joint)
{
    if ( (joint.getAcceptMask() & visitMask) == 0 ) {
        return;
    }

    visitTransform(joint);
    joint.skinningMatrix = joint.localToWorld * joint.inverseBindMatrix; 
}

const math::Matrix4f& TraverseVisitor::getLocalToWorldTransform() const 
{
    if ( transformNodeStack.empty() ) {
        return identityMatrix;
    }
    return transformNodeStack.back()->localToWorld;
}

const math::Matrix4f& TraverseVisitor::getWorldToLocalTransform() const 
{
    if ( transformNodeStack.empty() ) {
        return identityMatrix;
    }
    return transformNodeStack.back()->worldToLocal;
}

void TraverseVisitor::visitGroup(Group& group)
{
    for( Group::node_iterator i = group.firstChild();
                              i != group.endChild();
                              ++i )
    {
        if ( ((*i)->getAcceptMask() & visitMask) != 0 ) 
        {
            forTraverse.push_back( (*i).get() );
            transformDepthStack.push_back( transformNodeStack.size() );
        }
    }
}
