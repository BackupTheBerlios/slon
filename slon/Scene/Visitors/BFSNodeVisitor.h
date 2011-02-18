#ifndef __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__

#include "NodeVisitor.h"
#include <queue>

namespace slon {
namespace scene {

template<typename Node>
class BFSNodeVisitorImpl : 
    public NodeVisitorImpl<Node>
{
public:
    // Override NodeVisitor
    void traverse(Node& node);

    /** Visit node. */
    virtual void visitNode(Node& node) = 0;

    virtual ~BFSNodeVisitorImpl() {}

private:
    std::queue<Node*> forTraverse;
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_BFS_NODE_VISITOR_H__
