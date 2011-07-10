#ifndef __SLON_ENGINE_SCENE_VISITOR_VISITOR_IMPL_HPP__
#define __SLON_ENGINE_SCENE_VISITOR_VISITOR_IMPL_HPP__

#include "../AcceptVisitor.hpp"
#include "Traverse.hpp"
#include "Visitor.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace slon {
namespace scene {

template<typename VisitorT>
struct visit_func
{
    visit_func(VisitorT* visitor_)
    :   visitor(visitor_)
    {}

    void operator () (typename VisitorT::node_type& node) { visitor->visit(node); }

    VisitorT* visitor;
};

/** Implement visitor accept functionality. */
template<typename Derived, typename Base>
class VisitorImpl :
    public Base
{
BOOST_STATIC_ASSERT( (boost::is_base_of<Visitor, Base>::value) || (boost::is_base_of<ConstVisitor, Base>::value) );
public:
    typedef Base                        base_type;
    typedef typename Base::node_type    node_type;

    // tags
    typedef boost::true_type            accept_tag;

public:
    /// Traverse using DFS, call visit func on nodes, override if you wish to change default behaviour
    void traverse(node_type& node)
    {
        traverseDFS(node, visit_func<VisitorImpl>(this));
    }

    /** Call node accept function if one is presented. Then try to call accept function using reference to base visitor class. */
    void visit(node_type& node)
    {
        typedef typename if_then_else< boost::is_const<node_type>::value,
                                       const ConstAcceptVisitor<Derived>,
                                       AcceptVisitor<Derived> >::type    accept_visitor_type;

        if ( accept_visitor_type* accNode = dynamic_cast<accept_visitor_type*>(&node) ) {
            accNode->accept( static_cast<Derived&>(*this) );
        }
        else {
            base_type::visit(node);
        }
    }

    virtual ~VisitorImpl() {}
};

/** Implement visitor accept functionality. */
template<typename Derived>
class VisitorImpl<Derived, Visitor> :
    public Visitor
{
public:
    typedef Visitor                         base_type;
    typedef typename Visitor::node_type     node_type;

    // tags
    typedef boost::true_type                accept_tag;

public:
    /// Traverse using DFS, call visit func on nodes, override if you wish to change default behaviour
    void traverse(node_type& node)
    {
        traverseDFS(node, visit_func<VisitorImpl>(this));
    }

    /** Call node accept function if one is presented. Then try to call accept function using reference to base visitor class. */
    void visit(node_type& node)
    {
        typedef typename if_then_else< boost::is_const<node_type>::value,
                                       const ConstAcceptVisitor<Derived>,
                                       AcceptVisitor<Derived> >::type    accept_visitor_type;

        if ( accept_visitor_type* accNode = dynamic_cast<accept_visitor_type*>(&node) ) {
            accNode->accept( static_cast<Derived&>(*this) );
        }
    }

    virtual ~VisitorImpl() {}
};

/** Implement visitor accept functionality. */
template<typename Derived>
class VisitorImpl<Derived, ConstVisitor> :
    public ConstVisitor
{
public:
    typedef ConstVisitor                        base_type;
    typedef typename ConstVisitor::node_type    node_type;

    // tags
    typedef boost::true_type                    accept_tag;

public:
    /// Traverse using DFS, call visit func on nodes, override if you wish to change default behaviour
    void traverse(node_type& node)
    {
        traverseDFS(node, visit_func<VisitorImpl>(this));
    }

    /** Call node accept function if one is presented. Then try to call accept function using reference to base visitor class. */
    void visit(node_type& node)
    {
        typedef typename if_then_else< boost::is_const<node_type>::value,
                                       const ConstAcceptVisitor<Derived>,
                                       AcceptVisitor<Derived> >::type    accept_visitor_type;

        if ( accept_visitor_type* accNode = dynamic_cast<accept_visitor_type*>(&node) ) {
            accNode->accept( static_cast<Derived&>(*this) );
        }
    }

    virtual ~VisitorImpl() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_VISITOR_VISITOR_IMPL_HPP__
