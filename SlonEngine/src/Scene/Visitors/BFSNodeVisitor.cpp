#include "stdafx.h"
#include "Scene/Visitors/BFSNodeVisitor.h"
#include "Scene/Group.h"
#include "Utility/if_then_else.hpp"

namespace slon {
namespace scene {

template<typename Node>
void BFSNodeVisitorImpl<Node>::traverse(Node& node)
{
    forTraverse.push(&node);
    while ( !forTraverse.empty() )
    {
        Node* traversed = forTraverse.back(); forTraverse.pop();
        visitNode(*traversed);

        // add group children to traverse queue
        if (traversed->getNodeType() & scene::Node::GROUP) 
        {
            typedef if_then_else<boost::is_const<Node>::value, const Group, Group>::type Group;

            Group* group = static_cast<Group*>(traversed);
            for(Node* i = group->getChild(); i; i = i->getRight()) {
                forTraverse.push(i);
            }
        }
    }
}

// explicit instantiation
template class BFSNodeVisitorImpl<scene::Node>;
template class BFSNodeVisitorImpl<const scene::Node>;

} // namespace scene
} // namespace slon