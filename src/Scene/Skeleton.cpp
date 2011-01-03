#include "stdafx.h"
#include "Scene/Skeleton.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {

} // anonymous namespace

namespace slon {
namespace scene {

void Skeleton::setRootJoint(Joint* _root) 
{ 
    root.reset(_root);
}

void Skeleton::accept(scene::NodeVisitor& visitor)
{
    if (root) {
        root->accept(visitor);
    }
    visitor.visitNode(*this);
}

void Skeleton::accept(scene::TraverseVisitor& visitor)
{
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitSkeleton(*this);
}

void Skeleton::accept(scene::UpdateVisitor& visitor)
{
    if (root) {
        root->accept(visitor);
    }
    visitor.visitNode(*this);
}

void Skeleton::accept(scene::CullVisitor& visitor)
{
    if (cvCallback) {
        (*cvCallback)(*this, visitor);
    }
    visitor.visitNode(*this);
    // do not pass
}

} // namespace scene
} // namespace slon
