#include "stdafx.h"
#include "Physics/RigidBody.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletRigidBody.h"
#endif

namespace slon {
namespace physics {

RigidBody::RigidBody(const state_desc& desc_)
:   desc(desc_)
{
}

RigidBody::~RigidBody()
{
    // clear contacts
    if (world)
    {
        for (size_t i = 0; i<world->contacts.size(); ++i)
        {
            Contact& contact = world->contacts[i];
            if (contact.collisionObjects[0] == this || contact.collisionObjects[1] == this)
            {
                contact.collisionObjects[0]->handleDissappearingContact(contact);
                contact.collisionObjects[1]->handleDissappearingContact(contact);
                std::swap( contact, dynamicsWorld->contacts.back() );
                dynamicsWorld->contacts.pop_back();
                --i;
            }
        }

        // restore ordering
        std::sort( world->contacts.begin(), world->contacts.end(), compare_contact() );
    }
}

const char* RigidBody::serialize(database::OArchive& ar) const
{
    getStateDesc();
    ar.writeChunk("transform", desc.transform.data(), desc.transform.num_elements);
    ar.writeChunk("type", reinterpret_cast<const int*>(&desc.type));
    ar.writeChunk("mass", &desc.mass);
    ar.writeChunk("inertia", desc.inertia.arr, desc.inertia.num_elements);
    ar.writeChunk("margin", &desc.margin);
    ar.writeChunk("relativeMargin", &desc.relativeMargin);
    ar.writeChunk("linearVelocity", desc.linearVelocity.arr, desc.linearVelocity.num_elements);
    ar.writeChunk("angularVelocity", desc.angularVelocity.arr, desc.angularVelocity.num_elements);
    ar.writeStringChunk("name", desc.name.data(), desc.name.length());
    ar.writeStringChunk("target", desc.target.data(), desc.target.length());
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
    ar.readStringChunk("target", desc.target);
    desc.collisionShape = ar.readSerializable<CollisionShape>();
    reset(desc);
}

CollisionObject::COLLISION_TYPE RigidBody::getCollisionType() const
{
	return CT_RIGID_BODY;
}

const CollisionShape* RigidBody::getCollisionShape() const
{
	return desc.collisionShape.get();
}

const DynamicsWorld& RigidBody::getDynamicsWorld() const
{
	return *currentPhysicsManager().getDynamicsWorld();
}

const std::string& RigidBody::getName() const
{
	return desc.getName();
}

math::Matrix4r RigidBody::getTransform() const
{
	return impl->getMotionState()->getTransform();
}

void RigidBody::setTransform(const math::Matrix4r& transform)
{
	impl->getMotionState()->setTransform(transform);
}

void RigidBody::applyForce(const math::Vector3r& force, const math::Vector3r& pos)
{
    impl->applyForce(force, pos);
}

void RigidBody::applyTorque(const math::Vector3r& torque)
{
	impl->applyTorque(torque);
}

void RigidBody::applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos)
{
	impl->applyImpulse(impulse, pos);
}

void RigidBody::applyTorqueImpulse(const math::Vector3r& torqueImpulse)
{
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
	return impl->getActivationState();
}

void RigidBody::setActivationState(ACTIVATION_STATE state)
{
	impl->setActivationState(state);
}

math::Vector3r RigidBody::getLinearVelocity() const
{
	return impl->getLinearVelocity();
}

math::Vector3r RigidBody::getAngularVelocity() const
{
	return impl->getAngularVelocity();
}

const std::string& RigidBody::getName() const
{
	return desc.name;
}

DYNAMICS_TYPE RigidBody::getDynamicsType() const
{
	return desc.type;
}

const state_desc& RigidBody::getStateDesc() const
{
	desc.transform       = impl->getMotionState()->getTransform();
	desc.linearVelocity  = impl->getLinearVelocity();
	desc.angularVelocity = impl->getAngularVelocity();
	return desc;
}

RigidBodyTransform* RigidBody::getMotionState()
{
	return impl->getMotionState();
}

const RigidBodyTransform* RigidBody::getMotionState() const
{
	return impl->getMotionState();
}

void RigidBody::reset(const state_desc& desc)
{
	impl->reset(desc);
}

void RigidBody::toggleSimulation(bool toggle)
{
	impl->toggleSimulation(toggle);
}

RigidBody::constraint_iterator RigidBody::firstConstraint()
{
	return impl->firstConstraint();
}

RigidBody::constraint_iterator RigidBody::endConstraint()
{
	return impl->endConstraint();
}

void RigidBody::setWorld(const dynamics_world_ptr& world_)
{
	impl.reset();
    for (size_t i = 0; i<constraints.size(); ++i) {
        constraints[i]->release();
    }

    world = world_;
    if (world)
    {
        impl.reset( new BulletRigidBody(static_cast<BulletDynamicsWorld&>(*world), this) );
        for (size_t i = 0; i<constraints.size(); ++i) {
            constraints[i]->instantiate();
        }
    }
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
