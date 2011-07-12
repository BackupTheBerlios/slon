#include "stdafx.h"
#include "Physics/Constraint.h"

namespace slon {
namespace physics {

Constraint::Constraint(dynamics_world_ptr world_,
                       const state_desc&  desc_)
:   world(world_)
,   desc(desc_)
{
}

// Override Serializable
const char* Constraint::serialize(database::OArchive& ar) const
{
    ar.writeStringChunk("name", desc.name.data(), desc.name.length());
    ar.writeSerializable(desc.rigidBodies[0]);
    ar.writeSerializable(desc.rigidBodies[1]);
    ar.writeChunk("frame0", desc.frames[0].data(), desc.frames[0].num_elements);
    ar.writeChunk("frame1", desc.frames[1].data(), desc.frames[1].num_elements);
    ar.writeChunk("linearLimits0", desc.linearLimits[0].arr, desc.linearLimits[0].num_elements);
    ar.writeChunk("linearLimits0", desc.linearLimits[1].arr, desc.linearLimits[1].num_elements);
    ar.writeChunk("angularLimits0", desc.angularLimits[0].arr, desc.angularLimits[0].num_elements);
    ar.writeChunk("angularLimits0", desc.angularLimits[1].arr, desc.angularLimits[1].num_elements);
    return "BulletConstraint";
}

void Constraint::deserialize(database::IArchive& ar)
{
    ar.readStringChunk("name", desc.name);
    desc.rigidBodies[0] = ar.readSerializable<RigidBody>();
    desc.rigidBodies[1] = ar.readSerializable<RigidBody>();
    ar.readChunk("frame0", desc.frames[0].data(), desc.frames[0].num_elements);
    ar.readChunk("frame1", desc.frames[1].data(), desc.frames[1].num_elements);
    ar.readChunk("linearLimits0", desc.linearLimits[0].arr, desc.linearLimits[0].num_elements);
    ar.readChunk("linearLimits0", desc.linearLimits[1].arr, desc.linearLimits[1].num_elements);
    ar.readChunk("angularLimits0", desc.angularLimits[0].arr, desc.angularLimits[0].num_elements);
    ar.readChunk("angularLimits0", desc.angularLimits[1].arr, desc.angularLimits[1].num_elements);
    reset(desc);
}

RigidBody* Constraint::getRigidBodyA() const
{
	return desc.rigidBodies[0];
}

RigidBody* Constraint::getRigidBodyB() const
{
	return desc.rigidBodies[1];
}

const Motor* Constraint::getMotor(Motor::TYPE motor) const
{
	return motors[type];
}

Motor* Constraint::getMotor(Motor::TYPE motor)
{
	return motors[type];
}

ServoMotor* Constraint::createServoMotor(Motor::TYPE motor)
{
	assert( getRestriction(motor) != AXIS_LOCKED );
	motors[type].reset( new ServoMotor(motor) );
	return motors[type].get();
}

VelocityMotor* Constraint::createVelocityMotor(Motor::TYPE motor)
{
	assert( getRestriction(motor) != AXIS_LOCKED );
	motors[type].reset( new VelocityMotor(motor) );
	return motors[type].get();
}

SpringMotor* Constraint::createSpringMotor(Motor::TYPE motor)
{
	assert( getRestriction(motor) != AXIS_LOCKED );
	motors[type].reset( new SpringMotor(motor) );
	return motors[type].get();
}

math::Vector3r Constraint::getAxis(unsigned int axis) const
{
	return impl->getAxis();
}

real Constraint::getPosition(DOF dof) const
{
	return impl->getPosition(dof);
}

AXIS_RESTRICTION Constraint::getRestriction(DOF dof) const
{
	if (dof < 3) 
	{
		if ( desc.linearLimits[0][dof] == -std::numeric_limits<real>::infinity() && desc.linearLimits[1][dof] == std::numeric_limits<real>::infinity() ) {
			return AXIS_FREE;
		}
		else if (desc.linearLimits[0][dof] < desc.linearLimits[1][dof]) {
			return AXIS_RESTRICTED;
		}
	}
	else
	{
		int i = dof - 3;
		if ( desc.angularLimits[0][i] == -std::numeric_limits<real>::infinity() && desc.angularLimits[1][i] == std::numeric_limits<real>::infinity() ) {
			return AXIS_FREE;
		}
		else if (desc.angularLimits[0][i] < desc.angularLimits[1][i]) {
			return AXIS_RESTRICTED;
		}
	}

    return AXIS_LOCKED;
}

const std::string& Constraint::getName() const
{
	return desc.name;
}

const state_desc& Constraint::getStateDesc() const
{
	return desc;
}

const DynamicsWorld* Constraint::getDynamicsWorld() const
{
	return world.get();
}

void Constraint::reset(const state_desc& desc_)
{
	desc = desc_;
}

void Constraint::setWorld(const dynamics_world_ptr& world_)
{
    assert( desc.rigidBodies[0] && desc.rigidBodies[1] && "Constraint must specify affected rigid bodies."); 
	desc.rigidBodies[0]->removeConstraint(this);
	desc.rigidBodies[1]->removeConstraint(this);
	impl.reset();

    world = world_;
    if (world) 
    {
        desc.rigidBodies[0]->addConstraint(this);
        desc.rigidBodies[1]->addConstraint(this);
        if ( desc.rigidBodies[0]->getDynamicsWorld() && desc.rigidBodies[1]->getDynamicsWorld() ) {
            instantiate();
        }
    }

    for (int i = 0; i<6; ++i) 
    {
        if (motors[i]) {
            motors[i]->setWorld(world);
        }
    }
}

void Constraint::instantiate()
{
	if ( !impl && desc.rigidBodies[0]->getDynamicsWorld() && desc.rigidBodies[1]->getDynamicsWorld() ) 
	{
        assert( world == desc.rigidBodies[0]->getDynamicsWorld()
                && desc.rigidBodies[0]->getDynamicsWorld() == desc.rigidBodies[0]->getDynamicsWorld() 
                && "Linked bodies and constraint should be in same dynamics world." );
		impl.reset( new BulletConstraint(static_cast<BulletDynamicsWorld&>(*world), this) );
	}
}

void Constraint::release()
{
	impl.reset();
}

} // namespace physics
} // namespace slon
