#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__

#include "../Database/Serializable.h"
#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <map>
#include <set>

namespace slon {
namespace physics {

/** Container for rigid bodies, constraints and other physics entities. */
class PhysicsModel :
    public Referenced,
    public database::Serializable
{
public: 
    typedef std::map<collision_object_ptr, std::string>  collision_object_map;
    typedef collision_object_map::iterator               collision_object_iterator;
    typedef collision_object_map::const_iterator         collision_object_const_iterator;

    typedef std::set<constraint_ptr>                     constraint_set;
    typedef constraint_set::iterator                     constraint_iterator;
    typedef constraint_set::const_iterator               constraint_const_iterator;

public:
    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	/** Get name of the physics model */
	const std::string& getName() const { return name; }

	/** Set name of the physics model */
	void setName(const std::string& name_) { name = name_; }

    /** Add rigid body to the physics model. 
     * @param rigidBody - rigid body to store in the model.
     * @param target - name of the target node for rigid body.
     */
    void addCollisionObject(const collision_object_ptr& rigidBody, const std::string& target = "");

    /** Remove rigid body from the model. */
    bool removeCollisionObject(const collision_object_ptr& rigidBody);

    /** Add constraint to the physics model */
    void addConstraint(const constraint_ptr& constraint);

    /** Remove constraint from the physics model */
    bool removeConstraint(const constraint_ptr& constraint);

	/** Find collision object by name */
	collision_object_iterator findCollisionObjectByName(const std::string& name);
	
	/** Find collision object by target */
	collision_object_iterator findCollisionObjectByTarget(const std::string& target);

	/** Find constraint by name */
	constraint_iterator findConstraintByName(const std::string& name);

    /** Get iterator addressing first collision object. */
    collision_object_iterator firstCollisionObject() { return collisionObjects.begin(); }

    /** Get iterator addressing first collision object. */
    collision_object_const_iterator firstCollisionObject() const { return collisionObjects.begin(); }

    /** Get iterator addressing end of collision objects. */
    collision_object_iterator endCollisionObject() { return collisionObjects.end(); }

    /** Get iterator addressing end of collision objects. */
    collision_object_const_iterator endCollisionObject() const { return collisionObjects.end(); }

    /** Get iterator addressing first constraint. */
    constraint_iterator firstConstraint() { return constraints.begin(); }

    /** Get iterator addressing first constraint. */
    constraint_const_iterator firstConstraint() const { return constraints.begin(); }

    /** Get iterator addressing end of constraints. */
    constraint_iterator endConstraint() { return constraints.end(); }

    /** Get iterator addressing end of constraints. */
    constraint_const_iterator endConstraint() const { return constraints.end(); }

private:
	std::string          name;
    collision_object_map collisionObjects;
    constraint_set       constraints;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__
