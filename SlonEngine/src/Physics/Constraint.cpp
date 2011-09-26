#include "stdafx.h"
#include "Database/Archive.h"
#include "Physics/Constraint.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/RigidBody.h"
#include "Physics/ServoMotor.h"
#include "Physics/VelocityMotor.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletConstraint.h"
#endif

namespace slon {
namespace physics {

Constraint::Constraint(const DESC& desc_)
:   desc(desc_)
{
    if (desc.rigidBodies[0]) {
        desc.rigidBodies[0]->addConstraint(this);
    }
    if (desc.rigidBodies[1]) {
        desc.rigidBodies[1]->addConstraint(this);
    }
}

Constraint::~Constraint()
{
}

// Override Serializable
const char* Constraint::serialize(database::OArchive& ar) const
{
    ar.writeStringChunk("name", desc.name.data(), desc.name.length());
    ar.writeSerializable(desc.rigidBodies[0].get());
    ar.writeSerializable(desc.rigidBodies[1].get());
    ar.writeChunk("frame0", desc.frames[0].data(), desc.frames[0].num_elements);
    ar.writeChunk("frame1", desc.frames[1].data(), desc.frames[1].num_elements);
    ar.writeChunk("linearLimits0", desc.linearLimits[0].arr, desc.linearLimits[0].num_elements);
    ar.writeChunk("linearLimits1", desc.linearLimits[1].arr, desc.linearLimits[1].num_elements);
    ar.writeChunk("angularLimits0", desc.angularLimits[0].arr, desc.angularLimits[0].num_elements);
    ar.writeChunk("angularLimits1", desc.angularLimits[1].arr, desc.angularLimits[1].num_elements);
    ar.writeChunk("ignoreCollisions", &desc.ignoreCollisions);
    return "Constraint";
}

void Constraint::deserialize(database::IArchive& ar)
{
    if (desc.rigidBodies[0]) {
        desc.rigidBodies[0]->removeConstraint(this);
    }
    if (desc.rigidBodies[1]) {
        desc.rigidBodies[1]->removeConstraint(this);
    }
    ar.readStringChunk("name", desc.name);
    desc.rigidBodies[0] = ar.readSerializable<RigidBody>();
    if (desc.rigidBodies[0]) {
        desc.rigidBodies[0]->addConstraint(this);
    }
    desc.rigidBodies[1] = ar.readSerializable<RigidBody>();
    if (desc.rigidBodies[1]) {
        desc.rigidBodies[1]->addConstraint(this);
    }
    ar.readChunk("frame0", desc.frames[0].data(), desc.frames[0].num_elements);
    ar.readChunk("frame1", desc.frames[1].data(), desc.frames[1].num_elements);
    ar.readChunk("linearLimits0", desc.linearLimits[0].arr, desc.linearLimits[0].num_elements);
    ar.readChunk("linearLimits1", desc.linearLimits[1].arr, desc.linearLimits[1].num_elements);
    ar.readChunk("angularLimits0", desc.angularLimits[0].arr, desc.angularLimits[0].num_elements);
    ar.readChunk("angularLimits1", desc.angularLimits[1].arr, desc.angularLimits[1].num_elements);
    ar.readChunk("ignoreCollisions", &desc.ignoreCollisions);
    instantiate();
}

RigidBody* Constraint::getRigidBodyA() const
{
	return desc.rigidBodies[0].get();
}

const math::RigidTransformr& Constraint::getFrameInA() const
{
    return desc.frames[0];
}

RigidBody* Constraint::getRigidBodyB() const
{
	return desc.rigidBodies[1].get();
}

const math::RigidTransformr& Constraint::getFrameInB() const
{
    return desc.frames[1];
}

const Motor* Constraint::getMotor(DOF dof) const
{
	return motors[dof].get();
}

Motor* Constraint::getMotor(DOF dof)
{
	return motors[dof].get();
}

ServoMotor* Constraint::createServoMotor(DOF dof)
{
	assert( getRestriction(dof) != AXIS_LOCKED );
	motors[dof].reset( new ServoMotor(this, dof) );
	if (impl) {
		motors[dof]->instantiate();
	}
	return static_cast<ServoMotor*>(motors[dof].get());
}

VelocityMotor* Constraint::createVelocityMotor(DOF dof)
{
	assert( getRestriction(dof) != AXIS_LOCKED );
	motors[dof].reset( new VelocityMotor(this, dof) );
	if (impl) {
		motors[dof]->instantiate();
	}
	return static_cast<VelocityMotor*>(motors[dof].get());
}

SpringMotor* Constraint::createSpringMotor(DOF dof)
{
	assert( getRestriction(dof) != AXIS_LOCKED );
	//motors[dof].reset( new SpringMotor(this, dof) );
	//return motors[dof].get();
	return 0;
}

math::Vector3r Constraint::getAxis(unsigned int axis) const
{
	return impl->getAxis(axis);
}

real Constraint::getPosition(DOF dof) const
{
	return impl->getPosition(dof);
}

Constraint::AXIS_RESTRICTION Constraint::getRestriction(DOF dof) const
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

const Constraint::DESC& Constraint::getDesc() const
{
	return desc;
}

const DynamicsWorld* Constraint::getDynamicsWorld() const
{
	return world.get();
}

void Constraint::reset(const DESC& desc_)
{
    if (desc.rigidBodies[0]) {
        desc.rigidBodies[0]->removeConstraint(this);
    }
    if (desc.rigidBodies[1]) {
        desc.rigidBodies[1]->removeConstraint(this);
    }
	desc = desc_;
    if (desc.rigidBodies[0]) {
        desc.rigidBodies[0]->addConstraint(this);
    }
    if (desc.rigidBodies[1]) {
        desc.rigidBodies[1]->addConstraint(this);
    }
	instantiate();
}

void Constraint::setWorld(const dynamics_world_ptr& world_)
{
    assert( desc.rigidBodies[0] && desc.rigidBodies[1] && "Constraint must specify affected rigid bodies."); 
    release();
    world = world_;
    instantiate();
}

void Constraint::instantiate()
{
    if (!desc.rigidBodies[0] || !desc.rigidBodies[1]) {
        return;
    }

	if ( !impl && world && desc.rigidBodies[0]->getDynamicsWorld() && desc.rigidBodies[1]->getDynamicsWorld() ) 
	{
        assert( world == desc.rigidBodies[0]->getDynamicsWorld()
                && desc.rigidBodies[0]->getDynamicsWorld() == desc.rigidBodies[0]->getDynamicsWorld() 
                && "Linked bodies and constraint should be in same dynamics world." );
        impl.reset( new BulletConstraint(this, world->getImpl()) );
		for (int i = 0; i<6; ++i) 
		{
			if (motors[i]) {
				motors[i]->instantiate();
			}
		}
	}
}

void Constraint::release()
{
    for (int i = 0; i<6; ++i) 
    {
        if (motors[i]) {
            motors[i]->release();
        }
    }
    impl.reset();
}

} // namespace physics
} // namespace slon
