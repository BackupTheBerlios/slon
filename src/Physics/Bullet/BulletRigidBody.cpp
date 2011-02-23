#include "stdafx.h"
#include "Engine.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"

namespace slon {
namespace physics {

__DEFINE_LOGGER__("physics.BulletRigidBody")

namespace {

	btRigidBody::btRigidBodyConstructionInfo makeRigidBodyDesc(const RigidBody::state_desc& desc, btMotionState& motionState)
	{
		btCollisionShape* collisionShape = 0;
        btVector3         localInertia   = btVector3(0.0, 0.0, 0.0);
        if (desc.collisionShape)
		{
            real minDimension;
		    collisionShape = createBtCollisionShape(*desc.collisionShape, minDimension);
			collisionShape->setMargin(desc.relativeMargin * minDimension + desc.margin);
            
            if ( desc.mass > real(0.0) ) {
                collisionShape->calculateLocalInertia(desc.mass, localInertia);
            }
		}
		motionState.setWorldTransform( to_bt_mat(desc.transform) );
		
		btRigidBody::btRigidBodyConstructionInfo info(desc.mass, &motionState, collisionShape, localInertia);
		return info;
	}

} // anonymous namespace

std::ostream& operator << (std::ostream& os, const RigidBody::DYNAMICS_TYPE& type)
{
	os << (type == RigidBody::DT_DYNAMIC) ? "DT_DYNAMIC" : (type == RigidBody::DT_KINEMATIC) ? "DT_KINEMATIC" : "DT_STATIC";
	return os;
}

std::ostream& operator << (std::ostream& os, const BoxShape& shape)
{
	os << "box { halfExtent = {" << shape.halfExtents << "} }";
	return os;
}

std::ostream& operator << (std::ostream& os, const ConeShape& shape)
{
	os << "cone { radius = " << shape.radius << "; height = " << shape.height << " }";
	return os;
}

std::ostream& operator << (std::ostream& os, const CylinderShape& shape)
{
	os << "cylinder { halfExtent = {" << shape.halfExtent << "} }";
	return os;
}

std::ostream& operator << (std::ostream& os, const CollisionShape& shape)
{
	switch ( shape.getShapeType() )
	{
		case CollisionShape::PLANE:
		case CollisionShape::SPHERE:
		case CollisionShape::BOX:
			os << static_cast<const BoxShape&>(shape);
			break;

		case CollisionShape::CONE:
			os << static_cast<const ConeShape&>(shape);
			break;

		case CollisionShape::CAPSULE:
			break;

		case CollisionShape::CYLINDER:
			os << static_cast<const CylinderShape&>(shape);
			break;

		case CollisionShape::HEIGHTFIELD:
		case CollisionShape::CONVEX_MESH:
		case CollisionShape::TRIANGLE_MESH:
			break;

		case CollisionShape::COMPOUND:
		{
			const CompoundShape& cShape = static_cast<const CompoundShape&>(shape);
        
			for (size_t i = 0; i<cShape.shapes.size(); ++i) 
			{
				os << "compound\n"
				   << "{\n" 
				   << log::indent()
				   << "transform = \n{\n" << log::indent() << cShape.shapes[i].transform << log::unindent() << "\n}\n"
				   << "shape = " << *cShape.shapes[i].shape << std::endl
				   << log::unindent()
				   << "}";
			}

			break;
		}

		default:
			assert(!"can't get here");
	}

	return os;
}

std::ostream& operator << (std::ostream& os, const RigidBody::state_desc& desc)
{
	os << "{\n" 
	   << log::indent()
	   << "transform =\n{\n" << log::indent() << desc.transform << log::unindent() << "\n}\n"
	   << "type = " << desc.type << std::endl
	   << "mass = " << desc.mass << std::endl
	   << "margin = " << desc.margin << std::endl
	   << "relativeMargin = " << desc.relativeMargin << std::endl
	   << "linearVelocity = {" << desc.linearVelocity << "}\n"
	   << "angularVelocity = {" << desc.angularVelocity << "}\n"
	   << "name = " << desc.name << std::endl
	   << "target = " << desc.target << std::endl
	   << "shape = " << *desc.collisionShape << std::endl
	   << log::unindent()
	   << "}";

	return os;
}

BulletRigidBody::BulletRigidBody(const rigid_body_ptr rigidBody_,
                                 const std::string&   name_,
                                 DynamicsWorld*       dynamicsWorld_) :
    base_type(dynamicsWorld_),
    rigidBody(rigidBody_)
{
    assert(rigidBody);
	motionState.reset( new BulletMotionState(this) );
	{
		btTransform transform;
		if (btMotionState* ms = rigidBody->getMotionState() ) {
			ms->getWorldTransform(transform);
		}
		else {
			transform = rigidBody->getWorldTransform();
		}
		motionState->setWorldTransform(transform);
		rigidBody->setMotionState( motionState.get() );
	}
    rigidBody->setUserPointer(this);

    // fill up desc
    desc.transform = getTransform();
    if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT ) {
        desc.type = RigidBody::DT_KINEMATIC;
    }
    else if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT ) {
        desc.type = RigidBody::DT_STATIC;
    }
    else {
        desc.type = RigidBody::DT_DYNAMIC;
    }

    desc.mass            = real(1.0) / rigidBody->getInvMass();
    desc.linearVelocity  = to_vec( rigidBody->getLinearVelocity() );
    desc.angularVelocity = to_vec( rigidBody->getAngularVelocity() );
    desc.name            = name_;
	desc.target          = name_;
    
    CollisionShape* collisionShape = reinterpret_cast<CollisionShape*>( rigidBody->getCollisionShape()->getUserPointer() );
    if (!collisionShape) {
        collisionShape = createCollisionShape( *rigidBody->getCollisionShape() );
    }
    desc.collisionShape.reset(collisionShape);

	logger << log::S_FLOOD << "Creating rigid body from btRigidBody:\n" << desc << LOG_FILE_AND_LINE;
}

BulletRigidBody::BulletRigidBody(const RigidBody::state_desc& desc_, DynamicsWorld* dynamicsWorld_) :
	base_type(dynamicsWorld_)
{
	motionState.reset( new BulletMotionState(this) );
    reset(desc_);
}

BulletRigidBody::~BulletRigidBody()
{
    destroy();
}

void BulletRigidBody::applyForce(const math::Vector3r& force, const math::Vector3r& pos)
{
	rigidBody->applyForce( to_bt_vec(force), to_bt_vec(pos) );
}

void BulletRigidBody::applyTorque(const math::Vector3r& torque)
{
	rigidBody->applyTorque( to_bt_vec(torque) );
}

void BulletRigidBody::applyImpulse(const math::Vector3r& impulse, const math::Vector3r& pos)
{
	rigidBody->applyImpulse( to_bt_vec(impulse), to_bt_vec(pos) );
}

void BulletRigidBody::applyTorqueImpulse(const math::Vector3r& torqueImpulse)
{
	rigidBody->applyTorqueImpulse( to_bt_vec(torqueImpulse) );
}

math::Vector3r BulletRigidBody::getTotalForce() const
{
	// I wonder getTotalForce function is not const
	return to_vec( const_cast<btRigidBody&>(*rigidBody).getTotalForce() );
}

math::Vector3r BulletRigidBody::getTotalTorque() const
{
	// I wonder getTotalTorque function is not const
    return to_vec( const_cast<btRigidBody&>(*rigidBody).getTotalTorque() );
}

math::Matrix4r BulletRigidBody::getTransform() const
{
	btTransform worldTrans;
	motionState->getWorldTransform(worldTrans);
	return to_mat(worldTrans);
}

real BulletRigidBody::getMass() const
{
	return desc.type == DT_DYNAMIC ? 1.0f / rigidBody->getInvMass() : 0.0f;
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

math::Vector3r BulletRigidBody::getLinearVelocity() const
{
	return to_vec( rigidBody->getLinearVelocity() );
}

math::Vector3r BulletRigidBody::getAngularVelocity() const
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
        dynamicsWorld->getBtDynamicsWorld().removeRigidBody( rigidBody.get() );
        rigidBody.reset();

		logger << log::S_FLOOD << "Destroying rigid body" << LOG_FILE_AND_LINE;
    }
}

void BulletRigidBody::reset(const RigidBody::state_desc& desc_)
{
    // remove old rigid body
    destroy(false);

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

	logger << log::S_FLOOD << "Resetting rigid body:\n" << desc << LOG_FILE_AND_LINE;
}

void BulletRigidBody::setTransform(const math::Matrix4r& worldTransform)
{
    //rigidBody->setWorldTransform( to_bt_mat(worldTransform) );
    rigidBody->getMotionState()->setWorldTransform( to_bt_mat(worldTransform) );
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

} // namespace physics
} // namespace slon