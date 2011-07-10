#include "stdafx.h"
#include "Scene/Entity.h"
#include "Scene/Group.h"
#include "Scene/Visitor/CullVisitor.h"

namespace slon {
namespace scene {

void CullVisitor::traverse(const Node& node)
{
    forTraverse.push(&node);
    while ( !forTraverse.empty() )
    {
        const Node* traversed = forTraverse.top(); forTraverse.pop();

        // add group children to traverse queue
        if (traversed->getNodeType() & Node::ENTITY_BIT) {
            static_cast<const Entity*>(traversed)->accept(*this);
        }
        else
        {
            if (traversed->getNodeType() & Node::GROUP_BIT) 
            {
                const Group* group = static_cast<const Group*>(traversed);
                for(const Node* i = group->getChild(); i; i = i->getRight()) {
                    forTraverse.push(i);
                }
            }
            
            base_type::visit(*traversed);
        }
    }
}

void CullVisitor::clear()
{
    renderables.clear();
    lights.clear();
}

} // namespace scene
} // namespace slon