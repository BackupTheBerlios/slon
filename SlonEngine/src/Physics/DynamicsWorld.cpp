#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Physics/Detail/PhysicsManager.h"
#include "Physics/DynamicsWorld.h"
#include "Physics/RigidBody.h"
#ifdef SLON_ENGINE_USE_BULLET
#	include "Physics/Bullet/BulletDynamicsWorld.h"
#endif

namespace slon {
namespace physics {

DynamicsWorld::DynamicsWorld(const DESC& desc_)
:	desc(desc_)
{
	impl.reset( new BulletDynamicsWorld(this) );
}

DynamicsWorld::~DynamicsWorld()
{
}

void DynamicsWorld::setGravity(const math::Vector3r& gravity)
{
	desc.gravity = gravity;
	impl->setGravity(gravity);
}

math::Vector3r DynamicsWorld::getGravity() const
{
	return desc.gravity;
}

void DynamicsWorld::setFixedTimeStep(real dt)
{
	desc.fixedTimeStep = dt;
	impl->setFixedTimeStep(dt);
}

real DynamicsWorld::getFixedTimeStep() const
{
	return desc.fixedTimeStep;
}

void DynamicsWorld::setMaxNumSubSteps(unsigned nSteps)
{
	desc.maxNumSubSteps = nSteps;
	impl->setMaxNumSubSteps(nSteps);
}

unsigned DynamicsWorld::getMaxNumSubSteps() const
{
	return desc.maxNumSubSteps;
}

size_t DynamicsWorld::getNumSimulatedSteps() const
{
	return impl->getNumSimulatedSteps();
}

const DynamicsWorld::DESC& DynamicsWorld::getDesc() const
{
	return desc;
}

real DynamicsWorld::stepSimulation(real dt, bool force)
{
	return impl->stepSimulation(dt, force);
}

DynamicsWorld::contact_const_iterator DynamicsWorld::firstActiveContact() const
{
	return impl->firstActiveContact();
}

DynamicsWorld::contact_const_iterator DynamicsWorld::endActiveContact() const
{
	return impl->endActiveContact();
}

void DynamicsWorld::addRigidBody(RigidBody* rigidBody)
{
	rigidBody->setWorld(this);
}

void DynamicsWorld::addConstraint(Constraint* constraint)
{
	constraint->setWorld(this);
}

bool DynamicsWorld::removeRigidBody(RigidBody* rigidBody)
{
	assert(rigidBody);
	if ( rigidBody->getDynamicsWorld() == this ) 
	{
		rigidBody->setWorld(0);
		return true;
	}

	return false;
}

bool DynamicsWorld::removeConstraint(Constraint* constraint)
{
	assert(constraint);
	if ( constraint->getDynamicsWorld() == this ) 
	{
		constraint->setWorld(0);
		return true;
	}

	return false;
}

} // namespace physics
} // namespace slon
