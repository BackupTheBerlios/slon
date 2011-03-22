#ifndef SLON_ENGINE_SCENE_GRAPH_ENTITY_H
#define SLON_ENGINE_SCENE_GRAPH_ENTITY_H

#include "Group.h"
#include <sgl/Math/AABB.hpp>

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

    /** Get bounds of the entity */
    virtual const math::AABBf& getBounds() const = 0;

    /** Handle transform visitor */
    virtual void accept(TransformVisitor& cv) {}

    /** Handle cull visitor */
    virtual void accept(CullVisitor& cv) const {}

    virtual ~Entity() {}
};

typedef boost::intrusive_ptr<Entity>            entity_ptr;
typedef boost::intrusive_ptr<const Entity>      const_entity_ptr;

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_ENTITY_H
