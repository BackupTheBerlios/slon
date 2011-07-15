#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/CollisionObject.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/PhysicsTransform.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace slon {
namespace physics {

PhysicsTransform::PhysicsTransform(const collision_object_ptr& collisionObject_) :
    collisionObject(collisionObject_),
    absolute(false),
	lastNumSimulatedSteps(0)
{
	if (collisionObject) {
		transform = collisionObject->getTransform();
	}
	else {
		transform = math::make_identity<float, 4>();
	}
}

const char* PhysicsTransform::serialize(database::OArchive& ar) const
{
	// serialize base class
	Transform::serialize(ar);

	// serialize data
    ar.writeSerializable(collisionObject.get());
    ar.writeChunk("absolute", &absolute);
    return "PhysicsTransform";
}

void PhysicsTransform::deserialize(database::IArchive& ar)
{
	// deserialize base class
	Transform::deserialize(ar);

	// deserialize data
    collisionObject = ar.readSerializable<physics::CollisionObject>();
    ar.readChunk("absolute", &absolute);
}

const math::Matrix4f& PhysicsTransform::getTransform() const
{
	if (collisionObject)
	{
		if ( const DynamicsWorld* world = collisionObject->getDynamicsWorld() )
		{
			size_t numSimulatedSteps = world->getNumSimulatedSteps();
			if (numSimulatedSteps < lastNumSimulatedSteps)
			{
				transform             = collisionObject->getTransform();
				lastNumSimulatedSteps = numSimulatedSteps;
				const_cast<PhysicsTransform*>(this)->update();
			}
		}
	}

	return transform;
}

const math::Matrix4f& PhysicsTransform::getInverseTransform() const
{
	invTransform = math::invert( getTransform() );
	return invTransform;
}

void PhysicsTransform::accept(log::LogVisitor& visitor) const
{
    visitor << "PhysicsTransform";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "collisionObject =";
    if (collisionObject)
    {
        if ( collisionObject->getName() != "" ) {
            visitor << "'" << collisionObject->getName() << "'";
        }
        else {
            visitor << "unnamed(" << collisionObject << ")";
        }
    }
    else {
        visitor << "0";
    }
    visitor << "\n"
            << "transform =" << log::detailed(getTransform(), true)  
            << "localToWorld =" << log::detailed(getLocalToWorld(), true);
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}

void PhysicsTransform::accept(realm::EventVisitor& visitor)
{
    //if (ev.getType() == realm::EventVisitor::WORLD_ADD) {
    //    rigidBody->toggleSimulation(true);
    //}
    //else if (ev.getType() == realm::EventVisitor::WORLD_REMOVE) {
    //    rigidBody->toggleSimulation(false);
    //}
}

} // namespace physics
} // namespace slon
