#ifndef __SLON_ENGINE_SCENE_VISITOR_H__
#define __SLON_ENGINE_SCENE_VISITOR_H__

#include "Forward.h"
#include <boost/type_traits/integral_constant.hpp>

namespace slon {
namespace scene {

/** Base class for visitors, traversing scene graphs. */
class SLON_PUBLIC Visitor
{
public:
    typedef Node node_type;

    // tags
    typedef boost::false_type accept_tag;

public:
    /** Perform traverse. */
    virtual void traverse(node_type& node) = 0;

    virtual ~Visitor() {}
};

/** Base class for constant visitors, traversing scene graphs. */
class SLON_PUBLIC ConstVisitor
{
public:
    typedef const Node node_type;

    // tags
    typedef boost::false_type accept_tag;

public:
    /** Perform traverse. */
    virtual void traverse(node_type& node) = 0;

    virtual ~ConstVisitor() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_VISITOR_H__
