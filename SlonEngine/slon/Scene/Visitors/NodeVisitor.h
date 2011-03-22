#ifndef __SLON_ENGINE_SCENE_NODE_VISITOR_H__
#define __SLON_ENGINE_SCENE_NODE_VISITOR_H__

#include "../Forward.h"

namespace slon {
namespace scene {

/** Visitor traverses scene graph and operates on different kind of nodes */
template<typename Node>
class NodeVisitorImpl
{
public:
    typedef Node node_type;

public:
    /** Perform traverse. */
    virtual void traverse(Node& node) = 0;

    virtual ~NodeVisitorImpl() {}
};

} // namepsace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_NODE_VISITOR_H__
