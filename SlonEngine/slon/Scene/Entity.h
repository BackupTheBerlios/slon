#ifndef SLON_ENGINE_SCENE_GRAPH_ENTITY_H
#define SLON_ENGINE_SCENE_GRAPH_ENTITY_H

#include <sgl/Math/AABB.hpp>
#include "Group.h"
#include "Visitor/CullVisitor.h"
#include "Visitor/TransformVisitor.h"

namespace slon {
namespace scene {

/** Entity in the game world. Represents object that interacts with
 * game world.
 */
class Entity :
    public Node
{
public:
    // Override Node
    TYPE getNodeType() const { return ENTITY; }
    void accept(log::LogVisitor& visitor) const;

    /** Capture transform from visitor. */
    virtual void accept(TransformVisitor& /*visitor*/)  {}

    /** Add lights,renderables,cameras to cull visitor. */
    virtual void accept(CullVisitor& /*visitor*/) const {}

    /** Get bounds of the entity */
    virtual const math::AABBf& getBounds() const = 0;

    virtual ~Entity() {}
};

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_ENTITY_H
