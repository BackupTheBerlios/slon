#include "stdafx.h"
#include "Scene/Transform.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"

using namespace slon;
using namespace scene;

Transform::Transform() :
    traverseTS(0)
{
}

void Transform::accept(NodeVisitor& visitor)     
{ 
    if (nvCallback) {
        (*nvCallback)(*this, visitor); 
    }
    visitor.visitTransform(*this);
}

void Transform::accept(TraverseVisitor& visitor) 
{ 
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitTransform(*this);
}

void Transform::accept(UpdateVisitor& visitor)   
{ 
    if (uvCallback) {
        (*uvCallback)(*this, visitor);
    }
    visitor.visitTransform(*this);
}

void Transform::accept(CullVisitor& visitor)     
{ 
    if (cvCallback) {
        (*cvCallback)(*this, visitor);
    }
    visitor.visitTransform(*this);
}