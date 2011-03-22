#ifndef __SLON_ENGINE_SCENE_DFS_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_DFS_NODE_VISITOR_H__

#include "NodeVisitor.h"
#include <stack>

namespace slon {
namespace scene {

template<typename Node>
class DFSNodeVisitorImpl : 
    public NodeVisitorImpl<Node>
{
public:
    // Override NodeVisitor
    void traverse(Node& node);

    /** Visit node. */
    virtual void visitNode(Node& node) = 0;

    virtual ~DFSNodeVisitorImpl() {}

private:
    std::stack<Node*> forTraverse;
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_DFS_NODE_VISITOR_H__
