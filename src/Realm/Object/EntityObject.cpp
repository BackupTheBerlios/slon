#include "stdafx.h"
#include "Realm/Object/EntityObject.h"
#include "Scene/Visitors/NodeVisitor.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#	include "Physics/PhysicsModel.h"
#endif

namespace slon {
namespace realm {

EntityObject::EntityObject(scene::Entity& _entity, bool _dynamic) :
    dynamic(_dynamic),
    alwaysUpdate(false)
{
    world       = 0;
    location    = 0;
    spatialNode = 0;
    
    root.reset(&_entity);
}

void EntityObject::traverse(scene::NodeVisitor& nv)
{
    nv.traverse(*root);
}

void EntityObject::traverse(scene::ConstNodeVisitor& nv) const
{
    nv.traverse(*root);
}

#ifdef SLON_ENGINE_USE_PHYSICS
void EntityObject::setPhysicsModel(physics::PhysicsModel* physicsModel)
{

}
#endif

} // namesapce realm
} // namespace slon
