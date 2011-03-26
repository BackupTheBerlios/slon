#include "stdafx.h"
#include "Scene/Group.h"
#include "Scene/Visitors/DFSNodeVisitor.h"
#include "Utility/if_then_else.hpp"

namespace slon {
namespace scene {

template<typename Node>
void DFSNodeVisitorImpl<Node>::traverse(Node& node)
{
    forTraverse.push(&node);
    while ( !forTraverse.empty() )
    {
        Node* traversed = forTraverse.top(); forTraverse.pop();
        visitNode(*traversed);

        // add group children to traverse queue
        if (traversed->getNodeType() & scene::Node::GROUP) 
        {
            typedef typename if_then_else<boost::is_const<Node>::value, const Group, Group>::type Group;

            Group* group = static_cast<Group*>(traversed);
            for(Node* i = group->getChild(); i; i = i->getRight()) {
                forTraverse.push(i);
            }
        }
    }
}

// explicit instantiation
template class DFSNodeVisitorImpl<scene::Node>;
template class DFSNodeVisitorImpl<const scene::Node>;

} // namespace scene
} // namespace slon
