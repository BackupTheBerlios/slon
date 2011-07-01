#ifndef __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__

#include <queue>
#include "../../Utility/if_then_else.hpp"
#include "NodeVisitor.h"

namespace slon {
namespace scene {

template<typename Derived, typename Base>
class BFSNodeVisitor : 
    public NodeVisitorImpl<Derived, Base>
{
public:
    // Override NodeVisitor
    void traverse(Node& node)
    {    
        forTraverse.push(&node);
        while ( !forTraverse.empty() )
        {
            Node* traversed = forTraverse.back(); forTraverse.pop();
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

    /** Visit node. */
    virtual void visit(Node& node) = 0;

    virtual ~BFSNodeVisitorImpl() {}

private:
    std::queue<Node*> forTraverse;
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__
