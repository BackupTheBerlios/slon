#include "stdafx.h"
#include "Physics/Detail/PhysicsManager.h"
#include "Thread/Utility.h"

#ifdef SLON_ENGINE_USE_BULLET
#include "Physics/Bullet/BulletDynamicsWorld.h"
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

DynamicsWorld* PhysicsManager::initDynamicsWorld(const DynamicsWorld::state_desc& dynamicsWorldDesc)
{
    dynamicsWorld.reset( new DynamicsWorldImpl(dynamicsWorldDesc) );
    return dynamicsWorld.get();
}

void PhysicsManager::setTimer(const Timer* _timer)
{
    timer.reset(_timer);
    if (timer) {
        deltaTimer = delta_timer(_timer);
    }
}

void PhysicsManager::handlePhysics()
{
    assert(timer);
    if (!dynamicsWorld) {
        initDynamicsWorld();
    }

    preFrameSignal();
    {
        thread::lock_ptr lock = dynamicsWorld->lockForWriting();
        unsimulatedTime = dynamicsWorld->stepSimulation( (float)deltaTimer() + unsimulatedTime );
    }
    postFrameSignal();
}

} // namespace detail
} // namespace physics
} // namesapce slon

