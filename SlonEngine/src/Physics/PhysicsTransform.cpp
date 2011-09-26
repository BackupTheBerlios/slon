#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/RigidBody.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/PhysicsTransform.h"
#include "Realm/Location.h"
#include "Utility/functor_slot.hpp"

namespace slon {
namespace physics {

PhysicsTransform::PhysicsTransform(const collision_object_ptr& collisionObject_) :
    absolute(false),
    physicsTransform(0)
{
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
    if (collisionObject) {
        setCollisionObject(collisionObject);
    }
    ar.readChunk("absolute", &absolute);
}

const math::Matrix4f& PhysicsTransform::getTransform() const
{
#ifdef SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS
    return transform;
#else
    return physicsTransform ? (*physicsTransform) : transform;
#endif
}

const math::Matrix4f& PhysicsTransform::getInverseTransform() const
{
#ifdef SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS
    invTransform = math::invert(transform);
#else
    invTransform = math::invert(physicsTransform ? (*physicsTransform) : transform);
#endif
	return invTransform;
}

void PhysicsTransform::setCollisionObject(const collision_object_ptr& collisionObject_)
{
    collisionObject = collisionObject_;
    if (collisionObject)
    {
        transformConnection.reset( collisionObject->getTransformSignal(), 
                                   make_slot<void (const math::RigidTransformr&)>(boost::bind(&PhysicsTransform::setWorldTransform, this, _1)) );
        physicsTransform = collisionObject->getTransformPointer();
        transform = collisionObject->getTransform();
    }
    else {
        transformConnection.reset();
    }
}

void PhysicsTransform::setWorldTransform(const math::Matrix4f& transform_)
{
#ifdef SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS
    // copy if using double precision physics
    transform = math::RigidTransformf(transform_);
#else
     physicsTransform = collisionObject->getTransformPointer();
#endif
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
