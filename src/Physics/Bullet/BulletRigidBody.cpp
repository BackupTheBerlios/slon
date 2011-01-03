#include "stdafx.h"
#include "Engine.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"

using namespace slon;
using namespace physics;

namespace {

	inline btRigidBody::btRigidBodyConstructionInfo makeRigidBodyDesc(const RigidBody::state_desc& desc, btMotionState& motionState)
	{
		btCollisionShape* collisionShape = 0;
        btVector3         localInertia   = btVector3(0.0f, 0.0f, 0.0f);
        if (desc.collisionShape)
		{
		    collisionShape = createBtCollisionShape(*desc.collisionShape);
            
            if (desc.mass > 0.0f) {
                collisionShape->calculateLocalInertia(desc.mass, localInertia);
            }
		}
		motionState.setWorldTransform( to_bt_mat(desc.initialTransform) );
      
		btRigidBody::btRigidBodyConstructionInfo info(desc.mass, &motionState, collisionShape, localInertia);
		return info;
	}

} // anonymous namespace

BulletRigidBody::BulletRigidBody(const rigid_body_ptr rigidBody_,
                                 const std::string&   name_,
                                 DynamicsWorld*       dynamicsWorld_) :
    base_type(dynamicsWorld_),
    rigidBody(rigidBody_)
{
    assert(rigidBody);

    rigidBody->setUserPointer(this);
    if ( !rigidBody->getMotionState() ) {
        rigidBody->setMotionState(new btDefaultMotionState);
    }

    // fill up desc
    desc.initialTransform = getTransform();
    if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT ) {
        desc.type = RigidBody::DT_KINEMATIC;
    }
    else if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT ) {
        desc.type = RigidBody::DT_STATIC;
    }
    else {
        desc.type = RigidBody::DT_DYNAMIC;
    }
    desc.mass            = 1.0f / rigidBody->getInvMass();
    desc.linearVelocity  = to_vec( rigidBody->getLinearVelocity() );
    desc.angularVelocity = to_vec( rigidBody->getAngularVelocity() );
    desc.name            = name_;
    
    CollisionShape* collisionShape = reinterpret_cast<CollisionShape*>( rigidBody->getCollisionShape()->getUserPointer() );
    if (!collisionShape) {
        collisionShape = createCollisionShape( *rigidBody->getCollisionShape() );
    }
    desc.collisionShape.reset(collisionShape);
}

BulletRigidBody::BulletRigidBody(const RigidBody::state_desc& desc_, DynamicsWorld* dynamicsWorld_) :
	base_type(dynamicsWorld_)
{
    reset(desc_);
}

BulletRigidBody::~BulletRigidBody()
{
    destroy();
}

void BulletRigidBody::applyForce(const math::Vector3f& force, const math::Vector3f& pos)
{
	rigidBody->applyForce( to_bt_vec(force), to_bt_vec(pos) );
}

void BulletRigidBody::applyTorque(const math::Vector3f& torque)
{
	rigidBody->applyTorque( to_bt_vec(torque) );
}

void BulletRigidBody::applyImpulse(const math::Vector3f& impulse, const math::Vector3f& pos)
{
	rigidBody->applyImpulse( to_bt_vec(impulse), to_bt_vec(pos) );
}

void BulletRigidBody::applyTorqueImpulse(const math::Vector3f& torqueImpulse)
{
	rigidBody->applyTorqueImpulse( to_bt_vec(torqueImpulse) );
}

math::Vector3f BulletRigidBody::getTotalForce() const
{
	// I wonder getTotalForce function is not const
	return to_vec( const_cast<btRigidBody&>(*rigidBody).getTotalForce() );
}

math::Vector3f BulletRigidBody::getTotalTorque() const
{
	// I wonder getTotalTorque function is not const
    return to_vec( const_cast<btRigidBody&>(*rigidBody).getTotalTorque() );
}

math::Matrix4f BulletRigidBody::getTransform() const
{
	btTransform worldTrans;
	motionState->getWorldTransform(worldTrans);
	return to_mat(worldTrans);
}

float BulletRigidBody::getMass() const
{
	return 1.0f / rigidBody->getInvMass();
}

RigidBody::ACTIVATION_STATE BulletRigidBody::getActivationState() const
{
    switch ( rigidBody->getActivationState() )
    {
    case ACTIVE_TAG:
    case WANTS_DEACTIVATION:
        return AS_ACTIVE;

    case DISABLE_DEACTIVATION:
        return AS_DISABLE_DEACTIVATION;

    case DISABLE_SIMULATION:
        return AS_DISABLE_SIMULATION;

    case ISLAND_SLEEPING:
        return AS_SLEEPING;

    default:
        assert(!"Can't get here");
        return AS_ACTIVE;
    }
}

void BulletRigidBody::setActivationState(ACTIVATION_STATE state)
{
    switch (state)
    {
    case AS_ACTIVE:
        rigidBody->setActivationState(ACTIVE_TAG);
        break;

    case AS_DISABLE_DEACTIVATION:
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
        break;

    case AS_SLEEPING:
        rigidBody->setActivationState(ISLAND_SLEEPING);
        break;

    case AS_DISABLE_SIMULATION:
        rigidBody->setActivationState(DISABLE_SIMULATION);
        break;

    default:
        assert(!"Can't get here");
    }
}

math::Vector3f BulletRigidBody::getLinearVelocity() const
{
	return to_vec( rigidBody->getLinearVelocity() );
}

math::Vector3f BulletRigidBody::getAngularVelocity() const
{
	return to_vec( rigidBody->getAngularVelocity() );
}

void BulletRigidBody::destroy(bool unlinkConstraints)
{
    if (rigidBody) 
    {
        onDestroySignal(*this);

        // clear contacts
        {
            for (size_t i = 0; i<dynamicsWorld->contacts.size(); ++i)
            {
                Contact& contact = dynamicsWorld->contacts[i];
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
            std::sort( dynamicsWorld->contacts.begin(), dynamicsWorld->contacts.end(), compare_contact() );
        }

        // destroy constraints
        for (size_t i = 0; i<constraints.size(); ++i) {
            static_cast<BulletConstraint*>(constraints[i])->destroy();
        }

        // remove constraint referenced to this node
        if (unlinkConstraints) 
        {
            while (constraints.size() > 0) {
                static_cast<BulletConstraint*>(constraints[0])->unlink(*this);
            }
        }

        // destroy rigid body
        motionState.reset();
        dynamicsWorld->getBtDynamicsWorld().removeRigidBody( rigidBody.get() );
        rigidBody.reset();
    }
}

void BulletRigidBody::reset(const RigidBody::state_desc& desc_)
{
    // remove old rigid body
    destroy(false);

    // create new
    motionState.reset(new btDefaultMotionState);
	rigidBody.reset( new btRigidBody( makeRigidBodyDesc(desc_, *motionState) ) );
    rigidBody->setUserPointer(this);
	rigidBody->setLinearVelocity( to_bt_vec(desc_.linearVelocity) );
	rigidBody->setAngularVelocity( to_bt_vec(desc_.angularVelocity) );

    // static or dynamic rb
    if (desc_.type != RigidBody::DT_DYNAMIC)
    {
        int flags = desc_.type == RigidBody::DT_KINEMATIC ? btCollisionObject::CF_KINEMATIC_OBJECT : btCollisionObject::CF_STATIC_OBJECT;
        rigidBody->setCollisionFlags( rigidBody->getCollisionFlags() | flags );
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
    dynamicsWorld->getBtDynamicsWorld().addRigidBody( rigidBody.get() );

    // recreate constraints
    for (size_t i = 0; i<constraints.size(); ++i) {
        constraints[i]->reset( constraints[i]->getStateDesc() );
    }

    // remember
    desc = desc_;

    // call handlers
    onResetSignal(*this);
}

void BulletRigidBody::setTransform(const math::Matrix4f& worldTransform)
{
    //rigidBody->setWorldTransform( to_bt_mat(worldTransform) );
    motionState->setWorldTransform( to_bt_mat(worldTransform) );
}

void BulletRigidBody::removeConstraint(const Constraint& constraint)
{
    RigidBody::constraint_iterator iter = std::find(constraints.begin(), constraints.end(), &constraint);
    if ( iter != constraints.end() ) 
    {
        std::swap( *iter, constraints.back() );
        constraints.pop_back();
    }
}

BulletRigidBody::connection_type BulletRigidBody::connectResetHandler(rigid_body_signal::slot_type handler)
{
    return onResetSignal.connect(handler);
}

BulletRigidBody::connection_type BulletRigidBody::connectDestroyHandler(rigid_body_signal::slot_type handler)
{
    return onDestroySignal.connect(handler);
}
