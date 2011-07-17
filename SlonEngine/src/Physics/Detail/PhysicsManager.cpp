#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Detail/PhysicsManager.h"
#include "Thread/Utility.h"

#ifdef SLON_ENGINE_USE_BULLET
#   include "Physics/Bullet/BulletDynamicsWorld.h"
#endif

namespace slon {
namespace physics {
namespace detail {

#ifdef SLON_ENGINE_USE_BULLET
typedef BulletDynamicsWorld DynamicsWorldImpl;
#endif

PhysicsManager::PhysicsManager()
:   unsimulatedTime(0)
{
}

void PhysicsManager::handlePhysics()
{
    assert(timer);

    preFrameSignal();
    {
        //thread::lock_ptr lock = dynamicsWorld->lockForWriting();
        for (dynamics_world_iterator it  = firstDynamicsWorld();
                                     it != endDynamicsWorld();
                                     ++it)
        {
            (*it)->stepSimulation( (real)deltaTimer() );
        }
    }
    postFrameSignal();
}

void PhysicsManager::setTimer(const const_timer_ptr& timer_)
{
    timer = timer_;
}

void PhysicsManager::addDynamicsWorld(DynamicsWorld* world)
{
    worlds.push_back(world);
}

void PhysicsManager::removeDynamicsWorld(DynamicsWorld* world)
{
    worlds.remove(world);
}

} // namespace detail
} // namespace physics
} // namesapce slon

