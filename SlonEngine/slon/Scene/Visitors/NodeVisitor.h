#ifndef __SLON_ENGINE_SCENE_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_NODE_VISITOR_H__

#include "../Forward.h"

namespace slon {
namespace scene {

/** Base class for visitors, traversing scene graphs. */
class NodeVisitor
{
public:
    typedef Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(Node& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(Node* node);

    virtual ~NodeVisitor() {}
};

/** Base class for constant visitors, traversing scene graphs. */
class ConstNodeVisitor
{
public:
    typedef const Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(const Node& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(const Node* node);

    virtual ~ConstNodeVisitor() {}
};

/** Visitor class should be derived from this if it wants be acceptable by nodes */
template<typename Derived, typename Base>
class NodeVisitorImpl :
    public Base
{
public:
    typedef typename Base::node_type node_type;

public:
    /** Call node accept function if one is presented. Then try to call accept function using reference to base visitor class. */
    void visit(node_type* node)
    {
        typedef if_then_else< boost::is_const<node_type>::value,
                              const ConstAcceptVisitor<Derived>,
                              AcceptVisitor<Derived> >::type accept_visitor_type;

        if ( accept_visitor_type* accNode = dynamic_cast<accept_visitor_type*>(node) ) {
            accNode->accept( static_cast<Derived&>(*this) );
        }
        else {
            Base::visit(node);
        }
    }

    virtual ~NodeVisitorImpl() {}
};

/** traverse scene graph using depth first search. Call func for every traversed node. */
template<typename Func>
void traverse_dfs(Node& root, Func func)
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
void traverse_dfs(const Node& root, Func func)
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

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_NODE_VISITOR_H__
