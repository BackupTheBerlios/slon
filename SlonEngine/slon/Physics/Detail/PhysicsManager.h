#ifndef __SLON_ENGINE_PHYSICS_DETAIL_PHYSICS_MANAGER_H__
#define __SLON_ENGINE_PHYSICS_DETAIL_PHYSICS_MANAGER_H__

#include "../PhysicsManager.h"

namespace slon {
namespace physics {
namespace detail {

class PhysicsManager :
    public physics::PhysicsManager
{
public:
	PhysicsManager();

	/** Initialize physics dynamics world. */
	DynamicsWorld* initDynamicsWorld( const DynamicsWorld::state_desc& dynamicsWorldDesc = DynamicsWorld::state_desc() );

	/** Get dynamics world of the manager. */
	DynamicsWorld* getDynamicsWorld() const { return dynamicsWorld.get(); }

    /** Set timer for physics simulation. If timer is 0 simulation will stop. */
    void setTimer(const Timer* _timer);

    /** Get physics timer */
    const Timer* getTimer() const { return timer.get(); }

    /** Step simulation. This is called by engine. */
    void handlePhysics();

    /** Attach pre frame callback. Calls before physics frame simulation. */
    connection_type connectPreFrameCallback(pre_frame_signal::slot_type slot) {
        return preFrameSignal.connect(slot);
    }

    /** Attach post frame callback. Calls after physics frame simulation. */
    connection_type connectPostFrameCallback(post_frame_signal::slot_type slot) {
        return postFrameSignal.connect(slot);
    }

private:
	dynamics_world_ptr  dynamicsWorld;
    const_timer_ptr     timer;
    delta_timer         deltaTimer;
    real                unsimulatedTime;
    bool                realtime;

    // connections
    pre_frame_signal    preFrameSignal;
    post_frame_signal   postFrameSignal;
};

} // namespace detail
} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_DETAIL_PHYSICS_MANAGER_H__
