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
    enum ENTITY_TYPE
    {
        GEODE,
        LIGHT,
        CAMERA,
    };

public:
    /** Get type of the node */
    virtual TYPE getType() const { return ENTITY; }

    /** Get type of the entity */
    virtual ENTITY_TYPE getEntityType() const = 0;

    /** Get bounds of the entity */
    virtual const math::AABBf& getBounds() const = 0;

    // Override node
    using Node::accept;
    virtual void accept(NodeVisitor& visitor);
    virtual void accept(TraverseVisitor& visitor);
    virtual void accept(UpdateVisitor& visitor);
    virtual void accept(CullVisitor& visitor);

    virtual ~Entity() {}
};

typedef boost::intrusive_ptr<Entity>            entity_ptr;
typedef boost::intrusive_ptr<const Entity>      const_entity_ptr;

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_ENTITY_H
