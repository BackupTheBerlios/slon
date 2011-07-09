#ifndef __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_
#define __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_

#include "../Group.h"
#include "NodeVisitor.h"
#include <stack>

namespace slon {
namespace scene {

/** traverse scene graph using depth first search. Call func for every traversed node. */
template<typename Func>
void traverseDFS(Node& root, Func func)
{
    std::stack<Node*> forTraverse;
    forTraverse.push(&root);
    while ( !forTraverse.empty() )
    {
        Node* traversed = forTraverse.top(); forTraverse.pop();
        func(*traversed);

        // add group children to traverse queue
        if (traversed->getNodeType() & scene::Node::GROUP) 
        {
            Group* group = static_cast<Group*>(traversed);
            for(Node* i = group->getChild(); i; i = i->getRight()) {
                forTraverse.push(i);
            }
        }
    }
}

/** traverse scene graph using depth first search. Call func for every traversed node. */
template<typename Func>
void traverseDFS(const Node& root, Func func)
{
    std::stack<const Node*> forTraverse;
    forTraverse.push(&root);
    while ( !forTraverse.empty() )
    {
        const Node* traversed = forTraverse.top(); forTraverse.pop();
        func(*traversed);

        // add group children to traverse queue
        if (traversed->getNodeType() & scene::Node::GROUP) 
        {
            const Group* group = static_cast<const Group*>(traversed);
            for(Node* i = group->getChild(); i; i = i->getRight()) {
                forTraverse.push(i);
            }
        }
    }
}

/** traverse scene graph using depth first search. Call visitors visit func for every traversed node. */
template<typename Node, typename Visitor>
inline void traverseVisitorDFS( Visitor& visitor,
                                Node&    root, 
                                typename boost::is_convertible<Visitor&, const scene::NodeVisitor&>::type*   tag0 = 0,
                                typename boost::is_convertible<Node&, const scene::Node&>::type*             tag1 = 0)
{
    struct visit_func
    {
        visit_func(Visitor& visitor_)
        :   visitor(visitor_)
        {}
    
        void operator () (Node& node) { visitor.visit(&node); }

        Visitor& visitor;
    };

    traverseDFS(root, visit_func(visitor));
}

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_VISITORS_TRAVERSE_HPP_
