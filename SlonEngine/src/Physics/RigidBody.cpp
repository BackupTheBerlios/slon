#include "stdafx.h"
#include "Database/Archive.h"
#include "Physics/Constraint.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/RigidBody.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletRigidBody.h"
#endif

namespace slon {
namespace physics {

RigidBody::RigidBody(const DESC& desc_)
:   desc(desc_)
{
}

RigidBody::~RigidBody()
{
}

const char* RigidBody::serialize(database::OArchive& ar) const
{
    getDesc();
    ar.writeChunk("transform", desc.transform.data(), desc.transform.num_elements);
    ar.writeChunk("type", reinterpret_cast<const int*>(&desc.type));
    ar.writeChunk("mass", &desc.mass);
    ar.writeChunk("inertia", desc.inertia.arr, desc.inertia.num_elements);
    ar.writeChunk("margin", &desc.margin);
    ar.writeChunk("relativeMargin", &desc.relativeMargin);
    ar.writeChunk("linearVelocity", desc.linearVelocity.arr, desc.linearVelocity.num_elements);
    ar.writeChunk("angularVelocity", desc.angularVelocity.arr, desc.angularVelocity.num_elements);
    ar.writeStringChunk("name", desc.name.data(), desc.name.length());
    ar.writeSerializable(desc.collisionShape.get());
    return "RigidBody";
}

void RigidBody::deserialize(database::IArchive& ar)
{
    ar.readChunk("transform", desc.transform.data(), desc.transform.num_elements);
    ar.readChunk("type", reinterpret_cast<int*>(&desc.type));
    ar.readChunk("mass", &desc.mass);
    ar.readChunk("inertia", desc.inertia.arr, desc.inertia.num_elements);
    ar.readChunk("margin", &desc.margin);
    ar.readChunk("relativeMargin", &desc.relativeMargin);
    ar.readChunk("linearVelocity", desc.linearVelocity.arr, desc.linearVelocity.num_elements);
    ar.readChunk("angularVelocity", desc.angularVelocity.arr, desc.angularVelocity.num_elements);
    ar.readStringChunk("name", desc.name);
    desc.collisionShape = ar.readSerializable<CollisionShape>();
    reset(desc);
}

CollisionObject::COLLISION_TYPE RigidBody::getType() const
{
	return CT_RIGID_BODY;
}

const CollisionShape* RigidBody::getCollisionShape() const
{
	return desc.collisionShape.get();
}

const DynamicsWorld* RigidBody::getDynamicsWorld() const
{
	return world.get();
}

const std::string& RigidBody::getName() const
{
	return desc.name;
}

math::Matrix4r RigidBody::getTransform() const
{
	return impl ? impl->getTransform() : desc.transform;
}

void RigidBody::setTransform(const math::Matrix4r& transform)
{
    if (impl) {
	    impl->setTransform(transform);
    }
    else {
        desc.transform = transform;
    }
}

void RigidBody::applyForce(const math::Vector3r& force, const math::Vector3r& pos)
{
    assert(impl);
    impl->applyForce(force, pos);
}

void RigidBody::applyTorque(const math::Vector3r& torque)
{
    assert(impl);
	impl->applyTorque(torque);
}

void RigidBody::applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos)
{
    assert(impl);
	impl->applyImpulse(impulse, pos);
}

void RigidBody::applyTorqueImpulse(const math::Vector3r& torqueImpulse)
{
    assert(impl);
	impl->applyTorqueImpulse(torqueImpulse);
}

math::Vector3r RigidBody::getTotalForce() const
{
	return impl->getTotalForce();
}

math::Vector3r RigidBody::getTotalTorque() const
{
	return impl->getTotalTorque();
}

real RigidBody::getMass() const
{
	return desc.mass;
}

math::Vector3r RigidBody::getInertiaTensor() const
{
	return desc.inertia;
}

RigidBody::ACTIVATION_STATE RigidBody::getActivationState() const
{
    assert(impl);
	return impl->getActivationState();
}

void RigidBody::setActivationState(ACTIVATION_STATE state)
{
    assert(impl);
	impl->setActivationState(state);
}

math::Vector3r RigidBody::getLinearVelocity() const
{
    assert(impl);
	return impl->getLinearVelocity();
}

math::Vector3r RigidBody::getAngularVelocity() const
{
    assert(impl);
	return impl->getAngularVelocity();
}

RigidBody::DYNAMICS_TYPE RigidBody::getDynamicsType() const
{
	return desc.type;
}

const RigidBody::DESC& RigidBody::getDesc() const
{
	if (impl)
	{
		desc.transform       = impl->getTransform();
		desc.linearVelocity  = impl->getLinearVelocity();
		desc.angularVelocity = impl->getAngularVelocity();
	}
	return desc;
}

void RigidBody::reset(const DESC& desc_)
{
	release();
	desc = desc_;
	instantiate();
}

RigidBody::constraint_iterator RigidBody::firstConstraint()
{
	return constraints.begin();
}

RigidBody::constraint_iterator RigidBody::endConstraint()
{
	return constraints.end();
}

void RigidBody::setWorld(const dynamics_world_ptr& world_)
{
	release();
    world = world_;
	instantiate();
}

void RigidBody::instantiate()
{
    if (world)
    {
        impl.reset( new BulletRigidBody(this, world->getImpl()) );
        for (size_t i = 0; i<constraints.size(); ++i) {
            constraints[i]->instantiate();
        }
    }
}

void RigidBody::release()
{
    for (size_t i = 0; i<constraints.size(); ++i) {
        constraints[i]->release();
    }
    impl.reset();
}

void RigidBody::addConstraint(Constraint* constraint)
{
	constraints.push_back(constraint);
}

void RigidBody::removeConstraint(Constraint* constraint)
{
	quick_remove(constraints, constraint);
}

} // namespace physics
} // namespace slon
