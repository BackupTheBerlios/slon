#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__

#include "../Config.h"
#include "Constraint.h"
#include "RigidBody.h"
#include <set>

namespace slon {
namespace physics {

/** Set of rigid bodies, constraints and other physics models. */
class PhysicsModel :
	public Referenced
{
public: 
    typedef std::set<rigid_body_ptr>    rigid_body_set;
    typedef rigid_body_set::iterator    rigid_body_iterator;

    typedef std::set<constraint_ptr>    constraint_set;
    typedef constraint_set::iterator    constraint_iterator;

public:
    PhysicsModel();
    ~PhysicsModel();

	/** Get name of the physics model */
	const std::string& getName() const { return name; }

	/** Set name of the physics model */
	void setName(const std::string& name_) { name = name_; }

    /** Add rigid body to the physics model and all its constraints */
    bool addRigidBody(RigidBody* rigidBody);

    /** Remove rigid body with its constraints from the model */
    bool removeRigidBody(RigidBody* rigidBody);

    /** Add constraint to the physics model */
    bool addConstraint(Constraint* constraint);

    /** Remove constraint from the physics model */
    bool removeConstraint(Constraint* constraint);

    /** Get iterator addressing first rigid body. */
    rigid_body_iterator firstRigidBody() { return rigidBodies.begin(); }

    /** Get iterator addressing end of rigid bodies. */
    rigid_body_iterator endRigidBody() { return rigidBodies.end(); }

    /** Get iterator addressing first constraint. */
    constraint_iterator firstConstraint() { return constraints.begin(); }

    /** Get iterator addressing end of constraints. */
    constraint_iterator endConstraint() { return constraints.end(); }
    
    /** find rigid body by pointer. */
    rigid_body_iterator findRigidBody(RigidBody* rigidBody);

    /** find constraint by pointer. */
    constraint_iterator findConstraint(Constraint* constraint);

    /** find rigid body by the name. */
    rigid_body_iterator findRigidBody(const std::string& name);

    /** find constraint by the name. */
    constraint_iterator findConstraint(const std::string& name);

private:
	std::string			 name;
    rigid_body_set       rigidBodies;
    constraint_set       constraints;
};

typedef boost::intrusive_ptr<PhysicsModel>          physics_model_ptr;
typedef boost::intrusive_ptr<const PhysicsModel>    const_physics_model_ptr;

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_MODEL_H__
