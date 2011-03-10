#ifndef __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__
#define __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__

#include "../Object.h"

namespace slon {
namespace realm {
namespace detail {

/** Single scene object */
class Object :
	public realm::Object
{
public:
    Object(scene::Node* root    = 0, 
           bool         dynamic = false
#ifdef SLON_ENGINE_USE_PHYSICS
           , physics::PhysicsModel* physicsModel = 0
#endif
           );
    ~Object();

    /** Setup graphics model for the object */
    void setRoot(scene::Node* root);

    // Override Object
    bool                isDynamic() const			{ return dynamic; }
	void				toggleDynamic(bool toggle)	{ dynamic = toggle; }
    const math::AABBf&  getBounds() const			{ return aabb; }
    scene::Node*        getRoot()					{ return root.get(); }
    const scene::Node*  getRoot() const				{ return root.get(); }
    bool                isInWorld() const           { return location != 0; }

    void                traverse(scene::NodeVisitor& nv);
    void                traverse(scene::ConstNodeVisitor& nv) const;

#ifdef SLON_ENGINE_USE_PHYSICS
    void                         setPhysicsModel(physics::PhysicsModel* physicsModel);
    physics::PhysicsModel*       getPhysicsModel()          { return physicsModel.get(); }
    const physics::PhysicsModel* getPhysicsModel() const    { return physicsModel.get(); }

private:
    void clearPhysics(scene::Node* node);
#endif
	
public:
	Location*		getLocation()						{ return location; }
	const Location*	getLocation() const					{ return location; }
	void			setLocation(Location* location_)	{ location = location_; }

	void*			getLocationData()					 { return locationData; }
	void			setLocationData(void* locationData_) { locationData = locationData_; }

private:
    // spatial structure
    math::AABBf                 aabb;
	Location*					location;
	void*						locationData;
    bool                        dynamic;

	// data
	scene::node_ptr				root;
#ifdef SLON_ENGINE_USE_PHYSICS
    physics::physics_model_ptr  physicsModel;
#endif
};

typedef boost::intrusive_ptr<Object>		object_ptr;
typedef boost::intrusive_ptr<const Object>	const_object_ptr;

} // namespace detail
} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_COMPOUND_OBJECT_H__