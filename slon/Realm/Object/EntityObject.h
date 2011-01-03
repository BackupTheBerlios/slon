#ifndef __SLON_ENGINE_REALM_ENTITY_OBJECT_H__
#define __SLON_ENGINE_REALM_ENTITY_OBJECT_H__

#include "../../Scene/Entity.h"
#include "../Object.h"

namespace slon {
namespace realm {

/** Scene object from single entity. Contains implicit constructor from entity. */
class EntityObject :
    public Object
{
public:
    EntityObject(scene::Entity& entity, bool dynamic);

    // Override object
    bool                isDynamic() const    { return dynamic; }
    const math::AABBf&  getBounds() const    { return static_cast<const scene::Entity&>(*root).getBounds(); }
    const scene::Node*  getRoot() const      { return root.get(); }
    void                traverse(scene::NodeVisitor& nv);

#ifdef SLON_ENGINE_USE_PHYSICS
    void                         setPhysicsModel(physics::PhysicsModel* physicsModel);
    const physics::PhysicsModel* getPhysicsModel() const { return physicsModel.get(); }
#endif

private:
    // hint for spatial structure
    bool                        dynamic;
    bool                        alwaysUpdate;

#ifdef SLON_ENGINE_USE_PHYSICS
    physics::physics_model_ptr  physicsModel;
#endif
};

typedef boost::intrusive_ptr<EntityObject>          entity_object_ptr;
typedef boost::intrusive_ptr<const EntityObject>    const_entity_object_ptr;

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_ENTITY_OBJECT_H__