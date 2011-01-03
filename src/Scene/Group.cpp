#include "stdafx.h"
#include "Scene/Group.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Scene/Visitors/UpdateVisitor.h"

using namespace slon;
using namespace scene;

Group::const_node_iterator Group::findChild(Node& child) const
{
    return find( children.begin(), children.end(), node_ptr(&child) );
}

Group::node_iterator Group::findChild(Node& child)
{
    return find( children.begin(), children.end(), node_ptr(&child) );
}

void Group::moveChild(Group& group, Node& child)
{
    assert( child.parent == this && "Node is not child of this group" );

    node_iterator childIter = findChild(child);
    assert(childIter != children.end() && "Something is broken. Child parent is this node, but this node seems not to have this child.");
    moveChild(group, childIter);
}

void Group::moveChild(Group& group, const node_iterator& childIter)
{
    assert( childIter != children.end() && "Invalid childIter." );

    // add
    node_ptr child = *childIter;
    removeChild(childIter);
    group.addChild(*child);
}

void Group::addChild(Node& child)
{
    assert( child.parent == 0 && "Child node may not have any parent. If so, remove it from old parent first." );

    children.push_back( node_ptr(&child) );
    child.parent = this;
}

void Group::removeChild(Node& child)
{
    assert( child.parent == this && "Can't to remove child of another node." );

    node_iterator childIter = findChild(child);
    assert(childIter != children.end() && "Something is broken. Child parent is this node, but this node seems not to have this child.");
    removeChild(childIter);
}

void Group::removeChild(const node_iterator& childIter)
{
    assert( childIter != children.end() && "Invalid childIter." );
    assert( (*childIter)->parent == this && "Node is not child of this group." );

    (*childIter)->parent = 0;
    std::swap( *childIter, children.back() );
    children.pop_back();
}

void Group::replaceChild( Node& child,
                          Node& newChild )
{
    assert( child.parent == this && "Can't to remove child of another node." );
    assert( newChild.parent == 0 && "New child node may not have any parent. If so, remove it from old parent first." );

    node_iterator childIter = findChild(child);
    assert(childIter != children.end() && "Something is broken. Child parent is this node, but this node seems not to have this child.");
    replaceChild(childIter, newChild);
}

void Group::replaceChild( const node_iterator& childIter,
                          Node&                newChild )
{
    assert( childIter != children.end() && "Invalid childIter." );
    assert( (*childIter)->parent == this && "Node is not child of this group." );

    (*childIter)->parent = 0;
    childIter->reset(&newChild);
    newChild.parent = this;
}

Group::~Group()
{
    for(node_iterator i = children.begin(); i != children.end(); ++i) {
        (*i)->parent = 0;
    }
}

void Group::accept(NodeVisitor& visitor)     
{ 
    if (nvCallback) {
        (*nvCallback)(*this, visitor); 
    }
    visitor.visitGroup(*this);
}

void Group::accept(TraverseVisitor& visitor) 
{ 
    if (tvCallback) {
        (*tvCallback)(*this, visitor);
    }
    visitor.visitGroup(*this);
}

void Group::accept(UpdateVisitor& visitor)   
{ 
    if (uvCallback) {
        (*uvCallback)(*this, visitor);
    }
    visitor.visitGroup(*this);
}

void Group::accept(CullVisitor& visitor)     
{ 
    if (cvCallback) {
        (*cvCallback)(*this, visitor);
    }
    visitor.visitGroup(*this);
}