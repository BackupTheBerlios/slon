#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/RigidBody.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/PhysicsTransform.h"
#include "Realm/Location.h"
#include "Utility/functor_slot.hpp"
#include <sgl/Math/MatrixFunctions.hpp>

namespace slon {
namespace physics {

PhysicsTransform::PhysicsTransform(const collision_object_ptr& collisionObject_) :
    absolute(false)
{
	if (collisionObject_) {
		transform = collisionObject_->getTransform();
	}
	else {
		transform = math::make_identity<float, 4>();
	}
    setCollisionObject(collisionObject_);
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
	return transform;
}

const math::Matrix4f& PhysicsTransform::getInverseTransform() const
{
	invTransform = math::invert( getTransform() );
	return invTransform;
}

void PhysicsTransform::setCollisionObject(const collision_object_ptr& collisionObject_)
{
    collisionObject = collisionObject_;
    if (collisionObject) {
        transformConnection.reset( collisionObject->getTransformSignal(), make_slot<void (const math::Matrix4f&)>(boost::bind(&PhysicsTransform::setWorldTransform, this, _1)) );
    }
    else {
        transformConnection.reset();
    }
}

void PhysicsTransform::setWorldTransform(const math::Matrix4f& transform_)
{
    transform = transform_;
    ++modifiedCount;
    update();
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

void PhysicsTransform::accept(realm::EventVisitor& ev)
{
    if ( !ev.getPhysicsToggle() ) {
        return;
    }

	if ( realm::Location* location = ev.getLocation() )
	{
		if ( physics::DynamicsWorld* world = location->getDynamicsWorld() )
		{
			if (ev.getType() == realm::EventVisitor::WORLD_ADD) {
				world->addRigidBody( static_cast<RigidBody*>(collisionObject.get()) );
			}
			else if (ev.getType() == realm::EventVisitor::WORLD_REMOVE) {
				world->removeRigidBody( static_cast<RigidBody*>(collisionObject.get()) );
			}
		}
	}
}

} // namespace physics
} // namespace slon
