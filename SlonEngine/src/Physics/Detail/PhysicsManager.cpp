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

void PhysicsManager::addDynamicsWorld(const dynamics_world_ptr& world)
{
    worlds.push_back(world);
}

void PhysicsManager::removeDynamicsWorld(const dynamics_world_ptr& world)
{
    worlds.remove(world);
}

void PhysicsManager::handlePhysics()
{
    preFrameSignal();
    if (timer)
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
    if (timer) {
        deltaTimer = delta_timer(timer);
    }
}

} // namespace detail
} // namespace physics
} // namesapce slon

