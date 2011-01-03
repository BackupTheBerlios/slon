#ifndef SLON_ENGINE_SCENE_GRAPH_UPDATE_VISITOR_H
#define SLON_ENGINE_SCENE_GRAPH_UPDATE_VISITOR_H

#include "NodeVisitor.h"

namespace slon {
namespace scene {

/** Whether subgraph is relocated update visitor traverses it, forcing nodes
 * depending on the scene graph hierarchy to update their state.
 */
class UpdateVisitor :
    public NodeVisitor
{
public:
    // Override NodeVisitor
    virtual void acceptBy(Node& node) { node.accept(*this); }

    /** Move entity to the newer location if needed */
    virtual void visitEntity(Entity& entity);

    virtual ~UpdateVisitor() {}
};

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_UPDATE_VISITOR_H
