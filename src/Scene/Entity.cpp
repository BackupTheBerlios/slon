#include "stdafx.h"
#include "Scene/Entity.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"

using namespace slon;
using namespace scene;

void Entity::accept(NodeVisitor& visitor)     
{ 
    if (nvCallback) {
        (*nvCallback)(*this, visitor); 
    }
    visitor.visitEntity(*this);
}

void Entity::accept(TraverseVisitor& visitor) 
{ 
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitEntity(*this);
}

void Entity::accept(UpdateVisitor& visitor)   
{ 
    if (uvCallback) {
        (*uvCallback)(*this, visitor);
    }
    visitor.visitEntity(*this);
}

void Entity::accept(CullVisitor& visitor)     
{ 
    if (cvCallback) {
        (*cvCallback)(*this, visitor);
    }
    visitor.visitEntity(*this);
}