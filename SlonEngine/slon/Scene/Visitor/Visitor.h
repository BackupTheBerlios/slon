#ifndef __SLON_ENGINE_SCENE_VISITOR_VISITOR_H__
#define __SLON_ENGINE_SCENE_VISITOR_VISITOR_H__

#include "../Forward.h"

namespace slon {
namespace scene {

/** Base class for visitors, traversing scene graphs. */
class Visitor
{
public:
    typedef Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(node_type& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(node_type* node) {}

    virtual ~Visitor() {}
};

/** Base class for constant visitors, traversing scene graphs. */
class ConstVisitor
{
public:
    typedef const Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(node_type& node) = 0;

    /** Call node accept function if one is presented. */
    void visit(node_type* node) {}

    virtual ~ConstVisitor() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_VISITOR_VISITOR_H__
