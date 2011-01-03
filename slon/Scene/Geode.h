#ifndef __SLON_ENGINE_SCENE_GEODE_H__
#define __SLON_ENGINE_SCENE_GEODE_H__

#include "Entity.h"

namespace slon {
namespace scene {

/** Geode contains renderables.
 */
class Geode :
    public Entity
{
public:
    /** Get type of the entity */
    virtual ENTITY_TYPE getEntityType() const { return Entity::GEODE; }

    /** Gather renderables to be rendered.
     * @param renderPass - render pass wherer renderables have to be rendered.
     */
    //virtual void gatherRenderables(const graphics::RenderPass& renderPass) const = 0;

    /** Perform some operation on the renderables */
    //virtual void performOnRenderables(const RenderableFunctor& func) = 0;

    /** Perform some operation on the renderables */
    //virtual void performOnRenderables(const ConstRenderableFunctor& func) const = 0;

    virtual ~Geode() {}
};

typedef boost::intrusive_ptr<Geode>             geode_ptr;
typedef boost::intrusive_ptr<const Geode>       const_geode_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_GEODE_H__
