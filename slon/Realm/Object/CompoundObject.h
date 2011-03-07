#ifndef __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__
#define __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__

#include "../Object.h"

namespace slon {
namespace realm {

/** Single scene object */
class CompoundObject :
    public Object
{
public:
    CompoundObject(scene::Node* root    = 0, 
                   bool         dynamic = false
#ifdef SLON_ENGINE_USE_PHYSICS
                   , physics::PhysicsModel* physicsModel = 0
#endif
                    );
    ~CompoundObject();

    /** Setup graphics model for the object */
    void setRoot(scene::Node* root);

    // Override object
    bool                isDynamic() const   { return dynamic; }
    const math::AABBf&  getBounds() const   { return aabb; }
    scene::Node*        getRoot()           { return root.get(); }
    const scene::Node*  getRoot() const     { return root.get(); }

    void                traverse(scene::NodeVisitor& nv);
    void                traverse(scene::ConstNodeVisitor& nv) const;

#ifdef SLON_ENGINE_USE_PHYSICS
    void                         setPhysicsModel(physics::PhysicsModel* physicsModel);
    physics::PhysicsModel*       getPhysicsModel()          { return physicsModel.get(); }
    const physics::PhysicsModel* getPhysicsModel() const    { return physicsModel.get(); }

private:
    void clearPhysics(scene::Node* node);
#endif

private:
    // spatial structure
    math::AABBf                 aabb;

    // hint for spatial structure
    bool                        dynamic;
    bool                        alwaysUpdate;

#ifdef SLON_ENGINE_USE_PHYSICS
    physics::physics_model_ptr  physicsModel;
#endif
};

typedef boost::intrusive_ptr<CompoundObject>          compound_object_ptr;
typedef boost::intrusive_ptr<const CompoundObject>    const_compound_object_ptr;

} // namespace realm
} // namesapce slon

#endif // __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__