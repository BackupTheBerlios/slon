#include "stdafx.h"
#include "Scene/Group.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"

namespace slon {
namespace scene {

void Node::accept(NodeVisitor& visitor)     
{ 
    if (nvCallback) {
        (*nvCallback)(*this, visitor); 
    }
    visitor.visitNode(*this);
}

void Node::accept(TraverseVisitor& visitor) 
{ 
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitNode(*this);
}

void Node::accept(UpdateVisitor& visitor)   
{ 
    if (uvCallback) {
        (*uvCallback)(*this, visitor);
    }
    visitor.visitNode(*this);
}

void Node::accept(CullVisitor& visitor)     
{ 
    if (cvCallback) {
        (*cvCallback)(*this, visitor);
    }
    visitor.visitNode(*this);
}

Node* findNamedNode( Node& 			 root,
                     const std::string& name )
{
    if ( root.getName() == name ) {
        return &root;
    }

    if ( Group* group = dynamic_cast<Group*>(&root) )
    {
        for( Group::node_iterator i = group->firstChild();
                                  i != group->endChild();
                                  ++i )
        {
		    Node* node = findNamedNode(**i, name);
            if (node) {
                return node;
            }
	    }
    }

    // empty node
    return 0;
}

} // namespace slon
} // namespace scene
