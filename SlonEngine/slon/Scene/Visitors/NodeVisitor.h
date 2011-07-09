#ifndef __SLON_ENGINE_SCENE_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_NODE_VISITOR_H__

#include "../Forward.h"
#include "../Visitable.h"

namespace slon {
namespace scene {

/** Base class for visitors, traversing scene graphs. */
class NodeVisitor
{
public:
    typedef Node      node_type;
    typedef Visitable visitable_type;

public:
    /** Perform traverse. */
    virtual void traverse(Node& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(visitable_type* node) {}

    virtual ~NodeVisitor() {}
};

/** Base class for constant visitors, traversing scene graphs. */
class ConstNodeVisitor
{
public:
    typedef const Node      node_type;
    typedef const Visitable visitable_type;

public:
    /** Perform traverse. */
    virtual void traverse(const Node& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(visitable_type* node) {}

    virtual ~ConstNodeVisitor() {}
};

/** Visitor class should be derived from this if it wants be acceptable by nodes */
template<typename Derived, typename Base>
class NodeVisitorImpl :
    public Base
{
public:
    typedef typename Base::node_type        node_type;
    typedef typename Base::visitable_type   visitable_type;

public:
    /** Call node accept function if one is presented. Then try to call accept function using reference to base visitor class. */
    void visit(visitable_type* node)
    {
        typedef if_then_else< boost::is_const<visitable_type>::value,
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

/** traverse scene graph using depth first search. Call visitors visit func for every traversed node. */
template<typename Node, typename Visitor>
inline void visitor_traverse_dfs( Visitor& visitor,
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

    traverse_dfs(root, visit_func(visitor));
}

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_NODE_VISITOR_H__
