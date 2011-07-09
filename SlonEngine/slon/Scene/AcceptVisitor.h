#ifndef __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__
#define __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__

#include "../Utility/if_then_else.hpp"
#include "Visitable.h"
#include "Visitors/NodeVisitor.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace slon {
namespace scene {

/** Base class for nodes which accepts Visitor.  */
template<typename Visitor>
class AcceptVisitor :
    virtual public Visitable
{
BOOST_STATIC_ASSERT( (boost::is_base_of<NodeVisitor, Visitor>::value) );
public:
    /** Override this function to accept visitor. */
    virtual void accept(Visitor& visitor) = 0;

    virtual ~AcceptVisitor() {}
};

/** Base class for nodes which accepts Visitor. */
template<typename Visitor>
class ConstAcceptVisitor :
    virtual public Visitable
{
BOOST_STATIC_ASSERT( (boost::is_base_of<ConstNodeVisitor, Visitor>::value) );
public:
    /** Override this function to accept visitor. */
    virtual void accept(Visitor& visitor) const = 0;

    virtual ~ConstAcceptVisitor() {}
};


} // namespace scene
} // namespace slon

#endif __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__