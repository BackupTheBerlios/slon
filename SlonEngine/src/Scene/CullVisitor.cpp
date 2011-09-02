#include "stdafx.h"
//#include "Log/LogVisitor.h"
#include "Scene/Entity.h"
#include "Scene/Group.h"
#include "Scene/CullVisitor.h"

//DECLARE_AUTO_LOGGER("scene.CullVisitor");

namespace slon {
namespace scene {

void CullVisitor::traverse(const Node& node)
{
    //AUTO_LOGGER_INIT;
    //log::LogVisitor vis(AUTO_LOGGER, log::S_FLOOD, node);

    forTraverse.push(&node);
    while ( !forTraverse.empty() )
    {
        const Node* traversed = forTraverse.top(); forTraverse.pop();

        // add group children to traverse queue
        if (traversed->getNodeType() & Node::ENTITY_BIT) {
            static_cast<const Entity*>(traversed)->accept(*this);
        }
        else if (traversed->getNodeType() & Node::GROUP_BIT) 
        {
            const Group* group = static_cast<const Group*>(traversed);
            for(const Node* i = group->getChild(); i; i = i->getRight()) {
                forTraverse.push(i);
            }
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