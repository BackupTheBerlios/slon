#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__

#include "../Thread/Timer.h"
#include "DynamicsWorld.h"
#include <boost/signal.hpp>

namespace slon {
namespace physics {

class PhysicsManager
{
public:
    typedef boost::signal<void ()>          pre_frame_signal;
    typedef boost::signal<void ()>          post_frame_signal;
    typedef boost::signals::connection      connection_type;

public:
	/** Initialize physics dynamics world. */
	virtual DynamicsWorld* initDynamicsWorld( const DynamicsWorld::state_desc& dynamicsWorldDesc = DynamicsWorld::state_desc() ) = 0;

	/** Get dynamics world of the manager. */
	virtual DynamicsWorld* getDynamicsWorld() const = 0;

    /** Set timer for physics simulation. If timer is 0 simulation will stop. */
    virtual void setTimer(const Timer* timer) = 0;

    /** Get physics timer */
    virtual const Timer* getTimer() const = 0;

    /** Attach pre frame callback. Calls before physics frame simulation. */
    virtual connection_type connectPreFrameCallback(pre_frame_signal::slot_type slot) = 0;

    /** Attach post frame callback. Calls after physics frame simulation. */
    virtual connection_type connectPostFrameCallback(post_frame_signal::slot_type slot) = 0;

    virtual ~PhysicsManager() {}
};

/** Get current physics manager */
PhysicsManager& currentPhysicsManager();

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__
