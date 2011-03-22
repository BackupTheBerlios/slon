#include "stdafx.h"
#include "Log/LogVisitor.h"
#include "Scene/Group.h"

namespace slon {
namespace scene {

Node::Node() 
:   parent(0)
,	left(0)
,	right(0)
,	userPointer(0)
,	object(0)
{}

Node::Node(hash_string name_)
:	name(name_)
,	parent(0)
,	left(0)
,	right(0)
,	userPointer(0)
,	object(0)
{
}

void Node::accept(log::LogVisitor& visitor) const 
{
    if ( visitor.getLogger() )
    {
        visitor << "Node";
        if ( getName() != "" ) {
            visitor << " '" << getName() << "'";
        }
        visitor << " {}\n";
    }
}

Node* findNamedNode(Node& root, hash_string name)
{
    if ( root.getName() == name ) {
        return &root;
    }

    if ( Group* group = dynamic_cast<Group*>(&root) )
    {
        for(Node* i = group->getChild(); i; i = i->getRight())
        {
            if ( Node* node = findNamedNode(*i, name) ) {
                return node;
            }
	    }
    }

    // empty node
    return 0;
}

} // namespace slon
} // namespace scene
