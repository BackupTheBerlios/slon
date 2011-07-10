#ifndef __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__
#define __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__

#include "Forward.h"

namespace slon {
namespace scene {

/** Base class for nodes which accepts Visitor.  */
template<typename VisitorT>
class AcceptVisitor
{
public:
    /** Override this function to accept visitor. */
    virtual void accept(VisitorT& visitor) = 0;

    virtual ~AcceptVisitor() {}
};

/** Base class for nodes which accepts Visitor. */
template<typename VisitorT>
class ConstAcceptVisitor
{
public:
    /** Override this function to accept visitor. */
    virtual void accept(VisitorT& visitor) const = 0;

    virtual ~ConstAcceptVisitor() {}
};


} // namespace scene
} // namespace slon

#endif __SLON_ENGINE_SCENE_ACCEPT_VISITOR_H__