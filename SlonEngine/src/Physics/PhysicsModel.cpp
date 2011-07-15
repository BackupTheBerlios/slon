#include "stdafx.h"
#include "Database/Archive.h"
#include "Physics/CollisionObject.h"
#include "Physics/Constraint.h"
#include "Physics/PhysicsModel.h"

DECLARE_AUTO_LOGGER("physics.PhysicsModel")

namespace slon {
namespace physics {

const char* PhysicsModel::serialize(database::OArchive& ar) const
{
    ar.writeStringChunk("name", name.data(), name.length());
    ar.openChunk("collisionObjects");
	for (collision_object_map::const_iterator iter  = collisionObjects.begin(); 
		                                      iter != collisionObjects.end(); 
	                                          ++iter)
	{
		ar.openChunk("collision_object");
		ar.writeStringChunk( "target", iter->second.data(), iter->second.length() );
		ar.writeSerializable( iter->first.get() );
		ar.closeChunk();
	}
    ar.closeChunk();
    ar.openChunk("constraints");
    std::for_each(constraints.begin(), constraints.end(), boost::bind(&database::OArchive::writeSerializable, &ar, boost::bind(&constraint_ptr::get, _1), true, true));
    ar.closeChunk();

    return "PhysicsModel";
}

void PhysicsModel::deserialize(database::IArchive& ar)
{
    ar.readStringChunk("name", name);

    database::IArchive::chunk_info info;
    if ( !ar.openChunk("collisionObjects", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Missing collision objects chunk");
    }
	while ( ar.openChunk("collision_object", info) )
	{
		std::string target;
		ar.readStringChunk("target", target);
		collision_object_ptr rbody = ar.readSerializable<CollisionObject>(false, true);
		addCollisionObject(rbody, target);
		ar.closeChunk();
	}
    ar.closeChunk();

    if ( !ar.openChunk("constraints", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Missing constraints chunk");
    }
    while ( Constraint* constraint = ar.readSerializable<Constraint>(false, true) ) {
        constraints.insert( constraint_ptr(constraint) );
    }
    ar.closeChunk();
}

void PhysicsModel::addCollisionObject(const collision_object_ptr& co, const std::string& target)
{
    collisionObjects.insert( std::make_pair(co, target) );
}

bool PhysicsModel::removeCollisionObject(const collision_object_ptr& co)
{
	return (collisionObjects.erase(co) == 1);
}

void PhysicsModel::addConstraint(const constraint_ptr& constraint)
{
    constraints.insert(constraint);
}

bool PhysicsModel::removeConstraint(const constraint_ptr& constraint)
{
    return (constraints.erase(constraint) == 1);
}

PhysicsModel::collision_object_iterator PhysicsModel::findCollisionObjectByName(const std::string& name)
{
	for (collision_object_iterator iter  = collisionObjects.begin();
								   iter != collisionObjects.end();
								   ++iter)
	{
		if (iter->first->getName() == name) {
			return iter;
		}
	}

	return collisionObjects.end();
}

PhysicsModel::collision_object_iterator PhysicsModel::findCollisionObjectByTarget(const std::string& target)
{
	for (collision_object_iterator iter  = collisionObjects.begin();
								   iter != collisionObjects.end();
								   ++iter)
	{
		if (iter->second == target) {
			return iter;
		}
	}

	return collisionObjects.end();
}

PhysicsModel::constraint_iterator PhysicsModel::findConstraintByName(const std::string& name)
{
	for (constraint_iterator iter  = constraints.begin();
							 iter != constraints.end();
							 ++iter)
	{
		if ((*iter)->getName() == name) {
			return iter;
		}
	}

	return constraints.end();
}

} // namespace physics
} // namespace slon
