#ifndef __SLON_ENGINE_SCENE_FILTER_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_FILTER_NODE_VISITOR_H__

#include "../Forward.h"

namespace slon {
namespace scene {

template<typename T0, typename T1>
struct duo {};

template<typename Base,
         typename Tuple>
class FilterVisitorBase;

template<typename Base,
         typename T,
         typename Rest>
class FilterVisitorBase< Base, duo<T, Rest> > :
    public FilterVisitorBase<Base, Rest>
{
public:
    typedef FilterVisitorBase<Base, Rest>   base_type;
    typedef typename base_type::node_type   node_type;

public:
    virtual void visit(T& node) = 0;

protected:
    void visit(node_type& node)
    {
        if ( T* typedNode = dynamic_cast<T*>(&node) ) {
            visit(*typedNode);
        }
        else {
            base_type::visit(node);
        }
    }

    ~FilterVisitorBase() {}
};

template<typename Base,
         typename T>
class FilterVisitorBase< Base, duo<T, void> > :
    public Base
{
public:
    typedef Base                            base_type;
    typedef typename base_type::node_type   node_type;

public:
    virtual void visit(T& node) = 0;

protected:
    void visit(node_type& node)
    {
        if ( T* typedNode = dynamic_cast<T*>(&node) ) {
            visit(*typedNode);
        }
    }

    ~FilterVisitorBase() {}
};

template<typename Base,
         typename T>
class FilterVisitorBase< Base, duo<void, T> > :
    public Base
{
public:
    typedef Base                            base_type;
    typedef typename base_type::node_type   node_type;

protected:
    void visit(node_type&) {}

    ~FilterVisitorBase() {}
};

/** Visitor traverses scene graph and calls visit on nodes of specified types.
 * @tparam Base - base visitor type(BFSNodeVisitor, DFSNodeVisitor)
 */
template<typename Base,
         typename T0 = void,
         typename T1 = void,
         typename T2 = void,
         typename T3 = void,
         typename T4 = void,
         typename T5 = void, 
         typename T6 = void,
         typename T7 = void,
         typename T8 = void,
         typename T9 = void,
         typename TA = void,
         typename TB = void,
         typename TC = void,
         typename TD = void,
         typename TE = void,
         typename TF = void >
class FilterVisitor :
    public FilterVisitorBase<Base, duo<T0, duo<T1, duo<T2, duo<T3, duo<T4, duo<T5, duo<T6, duo<T7, duo<T8, duo<T9, duo<TA, duo<TB, duo<TC, duo<TD, duo<TE, TF> > > > > > > > > > > > > > > >
{
public:
    typedef FilterVisitorBase<Base, duo<T0, duo<T1, duo<T2, duo<T3, duo<T4, duo<T5, duo<T6, duo<T7, duo<T8, duo<T9, duo<TA, duo<TB, duo<TC, duo<TD, duo<TE, TF> > > > > > > > > > > > > > > > base_type;
    typedef typename Base::node_type node_type;

public:
    // Override Base
    void visitNode(node_type& node)
    {
        base_type::visit(node);
    }

    virtual ~FilterVisitor() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_FILTER_NODE_VISITOR_H__
