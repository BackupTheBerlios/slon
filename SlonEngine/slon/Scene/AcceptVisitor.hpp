#ifndef __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__
#define __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__

#include "Forward.h"
#include <boost/static_assert.hpp>

namespace slon {
namespace scene {

/** Base class for nodes which accepts Visitor. Only visitors with
 * accept_tag could be accepted. Standart visitors like CullVisitor
 * or TransformVisitor doesn't have accept_tag to avoid overhead
 * occuring due to accept functionality. If you wish to accept them - 
 * derive from Entity.
 */
template<typename VisitorT>
class AcceptVisitor
{
BOOST_STATIC_ASSERT( (VisitorT::accept_tag::value) );
public:
    /** Override this function to accept visitor. */
    virtual void accept(VisitorT& visitor) = 0;

    virtual ~AcceptVisitor() {}
};

/** Base class for nodes which accepts constant Visitor. Only visitors with
 * accept_tag could be accepted. Standart visitors like CullVisitor
 * or TransformVisitor doesn't have accept_tag to avoid overhead
 * occuring due to accept functionality. If you wish to accept them - 
 * derive from Entity.
 */
template<typename VisitorT>
class ConstAcceptVisitor
{
BOOST_STATIC_ASSERT( (VisitorT::accept_tag::value) );
public:
    /** Override this function to accept visitor. */
    virtual void accept(VisitorT& visitor) const = 0;

    virtual ~ConstAcceptVisitor() {}
};


} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__
