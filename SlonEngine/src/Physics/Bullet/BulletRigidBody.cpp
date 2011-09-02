#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Database/Archive.h"
#include "Engine.h"
#include "Log/Formatters.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletRigidBody.h"

DECLARE_AUTO_LOGGER("physics.BulletRigidBody")

namespace slon {
namespace physics {

class BulletMotionState :
    public btMotionState
{
public:
    BulletMotionState(BulletRigidBody* rbody_)
    :   rbody(rbody_)
    {
    }

	void getWorldTransform(btTransform& worldTrans_) const
    {
        worldTrans_ = worldTrans;
    }

	void setWorldTransform(const btTransform& worldTrans_)
    {
        worldTrans = worldTrans_;
        rbody->getTransformSignal()( to_mat(worldTrans) );
    }

private:
    BulletRigidBody* rbody;
    btTransform      worldTrans;
};

namespace {

	btRigidBody::btRigidBodyConstructionInfo makeRigidBodyDesc(RigidBody::DESC& desc, btMotionState& motionState)
	{
		btCollisionShape* collisionShape = 0;
        btVector3         localInertia   = to_bt_vec(desc.inertia);
		btTransform		  massFrame      = to_bt_mat(desc.transform);

        if (desc.collisionShape)
		{
		    collisionShape = createBtCollisionShape(*desc.collisionShape, desc.relativeMargin, desc.margin);
            if ( desc.mass > real(0.0) && localInertia.isZero() ) 
			{
                collisionShape->calculateLocalInertia(desc.mass, localInertia);
				desc.inertia = to_vec(localInertia);
			}
		}
		motionState.setWorldTransform(massFrame);

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
	os << "box { halfExtent = {" << shape.halfExtent << "} }";
	return os;
}

std::ostream& operator << (std::ostream& os, const ConeShape& shape)
{
	os << "cone { radius = " << shape.radius << "; height = " << shape.height << " }";
	return os;
}

std::ostream& operator << (std::ostream& os, const CylinderXShape& shape)
{
	os << "cylinderX { halfExtent = {" << shape.halfExtent << "} }";
	return os;
}

std::ostream& operator << (std::ostream& os, const CylinderYShape& shape)
{
	os << "cylinderY { halfExtent = {" << shape.halfExtent << "} }";
	return os;
}

std::ostream& operator << (std::ostream& os, const CylinderZShape& shape)
{
	os << "cylinderZ { halfExtent = {" << shape.halfExtent << "} }";
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

		case CollisionShape::CYLINDER_X:
			os << static_cast<const CylinderXShape&>(shape);
			break;

		case CollisionShape::CYLINDER_Y:
			os << static_cast<const CylinderYShape&>(shape);
			break;

		case CollisionShape::CYLINDER_Z:
			os << static_cast<const CylinderZShape&>(shape);
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

std::ostream& operator << (std::ostream& os, const RigidBody::DESC& desc)
{
	os << "{\n" 
	   << log::indent()
	   << "transform =\n{\n" << log::indent() << desc.transform << log::unindent() << "\n}\n"
	   << "type = " << desc.type << std::endl
	   << "mass = " << desc.mass << std::endl
	   << "inertia = " << desc.inertia << std::endl
	   << "margin = " << desc.margin << std::endl
	   << "relativeMargin = " << desc.relativeMargin << std::endl
	   << "linearVelocity = {" << desc.linearVelocity << "}\n"
	   << "angularVelocity = {" << desc.angularVelocity << "}\n"
	   << "name = " << desc.name << std::endl
       << "shape = ";
    
    if (desc.collisionShape) {
        os << *desc.collisionShape;
    }
    else {
        os << "0";
    }

    os << std::endl
	   << log::unindent()
	   << "}";

	return os;
}

BulletRigidBody::BulletRigidBody(RigidBody*           pInterface_,
                                 BulletDynamicsWorld* dynamicsWorld_,
	                             btRigidBody*         rigidBody_,
                                 const std::string&   name_ ) :
    base_type(pInterface, dynamicsWorld_),
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
    rigidBody->setUserPointer( pInterface );

    // fill up desc
	pInterface_->desc.transform = getTransform();
    if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT ) {
        pInterface_->desc.type = RigidBody::DT_KINEMATIC;
    }
    else if ( rigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT ) {
        pInterface_->desc.type = RigidBody::DT_STATIC;
    }
    else {
        pInterface_->desc.type = RigidBody::DT_DYNAMIC;
    }

    pInterface_->desc.mass            = real(1.0) / rigidBody->getInvMass();
    pInterface_->desc.linearVelocity  = to_vec( rigidBody->getLinearVelocity() );
    pInterface_->desc.angularVelocity = to_vec( rigidBody->getAngularVelocity() );
    pInterface_->desc.name            = name_;
    
    CollisionShape* collisionShape = reinterpret_cast<CollisionShape*>( rigidBody->getCollisionShape()->getUserPointer() );
    if (!collisionShape) {
        collisionShape = createCollisionShape( *rigidBody->getCollisionShape() );
    }
    pInterface_->desc.collisionShape.reset(collisionShape);

	AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Creating rigid body from btRigidBody:\n" << pInterface_->desc << LOG_FILE_AND_LINE);
}

BulletRigidBody::BulletRigidBody(RigidBody*             pInterface_,
								 BulletDynamicsWorld*   dynamicsWorld_)
:	base_type(pInterface_, dynamicsWorld_)
,   pInterface(pInterface_)
{
	RigidBody::DESC& desc = pInterface_->desc;
	motionState.reset( new BulletMotionState(this) );
    motionState->setWorldTransform( to_bt_mat(desc.transform) );

	rigidBody.reset( new btRigidBody( makeRigidBodyDesc(desc, *motionState) ) );
    rigidBody->setUserPointer( pInterface );
	rigidBody->setLinearVelocity( to_bt_vec(desc.linearVelocity) );
	rigidBody->setAngularVelocity( to_bt_vec(desc.angularVelocity) );

    // static or dynamic rb
    if (desc.type != RigidBody::DT_DYNAMIC)
    {
        int flags = desc.type == RigidBody::DT_KINEMATIC ? btCollisionObject::CF_KINEMATIC_OBJECT : btCollisionObject::CF_STATIC_OBJECT;
        rigidBody->setCollisionFlags( rigidBody->getCollisionFlags() | flags );
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }

	dynamicsWorld->getBtDynamicsWorld().addRigidBody( rigidBody.get() );
    AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Creating rigid body:\n" << desc << LOG_FILE_AND_LINE);
}

BulletRigidBody::~BulletRigidBody()
{
    // clear contacts
    for (size_t i = 0; i<dynamicsWorld->contacts.size(); ++i)
    {
        Contact& contact = dynamicsWorld->contacts[i];
        if (contact.collisionObjects[0] == pInterface || contact.collisionObjects[1] == pInterface)
        {
            if ( BulletCollisionObject* co = contact.collisionObjects[0]->getImpl() ) {
                co->getContactDissapearSignal()(contact);
            }
            if ( BulletCollisionObject* co = contact.collisionObjects[1]->getImpl() ) {
                co->getContactDissapearSignal()(contact);
            }
            std::swap( contact, dynamicsWorld->contacts.back() );
            dynamicsWorld->contacts.pop_back();
            --i;
        }
    }

    // restore ordering
    std::sort( dynamicsWorld->contacts.begin(), dynamicsWorld->contacts.end(), compare_contact() );

	dynamicsWorld->getBtDynamicsWorld().removeRigidBody( rigidBody.get() );
	AUTO_LOGGER_MESSAGE(log::S_FLOOD, "Destroying rigid body" << LOG_FILE_AND_LINE);
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

RigidBody::ACTIVATION_STATE BulletRigidBody::getActivationState() const
{
    switch ( rigidBody->getActivationState() )
    {
    case ACTIVE_TAG:
    case WANTS_DEACTIVATION:
        return RigidBody::AS_ACTIVE;

    case DISABLE_DEACTIVATION:
        return RigidBody::AS_DISABLE_DEACTIVATION;

    case DISABLE_SIMULATION:
        return RigidBody::AS_DISABLE_SIMULATION;

    case ISLAND_SLEEPING:
        return RigidBody::AS_SLEEPING;

    default:
        assert(!"Can't get here");
        return RigidBody::AS_ACTIVE;
    }
}

void BulletRigidBody::setActivationState(RigidBody::ACTIVATION_STATE state)
{
    switch (state)
    {
    case RigidBody::AS_ACTIVE:
        rigidBody->setActivationState(ACTIVE_TAG);
        break;

    case RigidBody::AS_DISABLE_DEACTIVATION:
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
        break;

    case RigidBody::AS_SLEEPING:
        rigidBody->setActivationState(ISLAND_SLEEPING);
        break;

    case RigidBody::AS_DISABLE_SIMULATION:
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

void BulletRigidBody::setTransform(const math::Matrix4r& worldTransform)
{
    motionState->setWorldTransform( to_bt_mat(worldTransform) );
}

} // namespace physics
} // namespace slon