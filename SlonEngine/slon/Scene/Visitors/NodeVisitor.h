#ifndef __SLON_ENGINE_SCENE_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_NODE_VISITOR_H__

#include "../../Utility/if_then_else.hpp"
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
    virtual void traverse(node_type& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(node_type* node) {}

    virtual ~NodeVisitor() {}
};

/** Base class for constant visitors, traversing scene graphs. */
class ConstNodeVisitor
{
public:
    typedef const Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(node_type& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(node_type* node) {}

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
        typedef typename if_then_else< boost::is_const<node_type>::value,
                                       const ConstAcceptVisitor<Derived>,
                                       AcceptVisitor<Derived> >::type    accept_visitor_type;

        if ( accept_visitor_type* accNode = dynamic_cast<accept_visitor_type*>(node) ) {
            accNode->accept( static_cast<Derived&>(*this) );
        }
        else {
            Base::visit(node);
        }
    }

    virtual ~NodeVisitorImpl() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_NODE_VISITOR_H__
