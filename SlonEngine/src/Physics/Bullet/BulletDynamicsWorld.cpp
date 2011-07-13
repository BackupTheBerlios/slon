#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/CollisionObject.h"
#include <sgl/Math/Utility.hpp>

using namespace slon;
using namespace physics;

BulletDynamicsWorld::BulletDynamicsWorld(DynamicsWorld* pInterface_)
:	pInterface(pInterface_)
,	numSimulatedSteps(0)
{
    const state_desc& desc = pInterface->desc;

    // if world have zero size autodetect
    if ( length(desc.worldSize.maxVec - desc.worldSize.minVec) < math::EPS_3f ) {
        broadPhase.reset( new btDbvtBroadphase() );
    }
    else {
        broadPhase.reset( new btAxisSweep3( to_bt_vec(desc.worldSize.minVec), to_bt_vec(desc.worldSize.maxVec) ) );
    }

    collisionConfiguration.reset( new btDefaultCollisionConfiguration() );
    collisionDispatcher.reset( new btCollisionDispatcher( collisionConfiguration.get() ) );
    constraintSolver.reset( new btSequentialImpulseConstraintSolver() );

    if (desc.collisionType == DynamicsWorld::CT_DISCRETE)
    {
        dynamicsWorld.reset( new btDiscreteDynamicsWorld( collisionDispatcher.get(),
                                                          broadPhase.get(),
                                                          constraintSolver.get(),
                                                          collisionConfiguration.get() ) );
    }
    else
    {/*
        dynamicsWorld.reset( new btContinuousDynamicsWorld( collisionDispatcher.get(),
                                                            broadPhase.get(),
                                                            constraintSolver.get(),
                                                            collisionConfiguration.get() ) );*/
    }

    dynamicsWorld->setGravity( to_bt_vec(desc.gravity) );
}

void BulletDynamicsWorld::setGravity(const math::Vector3r& gravity)
{
	dynamicsWorld->setGravity( to_bt_vec(gravity) );
}

real BulletDynamicsWorld::stepSimulation(real dt)
{
    if (dt < 0) {
        return 0;
    }

    real t = 0;
    {
        const state_desc& desc = pInterface->desc;
        for (unsigned i = 0; i<desc.maxNumSubSteps && (dt - t) >= desc.fixedTimeStep; ++i, t += desc.fixedTimeStep, ++numSimulatedSteps)
        {
            dynamicsWorld->stepSimulation(desc.fixedTimeStep, 1, desc.fixedTimeStep);
        }
    }

    // enumerate contacts
    contact_vector              currentContacts;
    std::vector<math::Vector3r> currentContactPoints;
    for (int i = 0; i<collisionDispatcher->getNumManifolds(); ++i)
    {
        btPersistentManifold*   contactManifold = collisionDispatcher->getManifoldByIndexInternal(i);
        btCollisionObject*      objA            = static_cast<btCollisionObject*>( contactManifold->getBody0() );
        btCollisionObject*      objB            = static_cast<btCollisionObject*>( contactManifold->getBody1() );

        currentContacts.push_back( Contact( static_cast<CollisionObject*>( objA->getUserPointer() ), 
                                            static_cast<CollisionObject*>( objB->getUserPointer() ) ) );
        
        currentContacts.back().numContacts = contactManifold->getNumContacts();
        for (int j = 0; j<contactManifold->getNumContacts(); ++j) {
            currentContactPoints.push_back( to_vec( contactManifold->getContactPoint(j).getPositionWorldOnB() ) );
        }
    }

    // fill pointers
    if ( !currentContactPoints.empty() )
    {
        math::Vector3r* contacts = &currentContactPoints[0];
        for (size_t i = 0; i<currentContacts.size(); ++i) 
        {
            currentContacts[i].contacts = contacts;
            contacts += currentContacts[i].numContacts;
        }
    }

    // enumerate appearing and dissapearing contacts
    contact_vector appearingContacts, dissapearingContacts;

    std::sort( currentContacts.begin(), currentContacts.end(), compare_contact() );
    std::set_difference( currentContacts.begin(), 
                         currentContacts.end(), 
                         contacts.begin(), 
                         contacts.end(), 
                         std::back_inserter(appearingContacts),
                         compare_contact() );
    std::set_difference( contacts.begin(), 
                         contacts.end(), 
                         currentContacts.begin(),
                         currentContacts.end(), 
                         std::back_inserter(dissapearingContacts),
                         compare_contact() );
    contacts.swap(currentContacts);
    contactPoints.swap(currentContactPoints);

    // call callbacks
    for (size_t i = 0; i<appearingContacts.size(); ++i) 
    {
        appearingContacts[i].collisionObjects[0]->handleAppearingContact(appearingContacts[i]);
        appearingContacts[i].collisionObjects[1]->handleAppearingContact(appearingContacts[i]);
    }

    for (size_t i = 0; i<dissapearingContacts.size(); ++i) 
    {
        dissapearingContacts[i].collisionObjects[0]->handleDissappearingContact(dissapearingContacts[i]);
        dissapearingContacts[i].collisionObjects[1]->handleDissappearingContact(dissapearingContacts[i]);
    }

    return dt - t;
}
