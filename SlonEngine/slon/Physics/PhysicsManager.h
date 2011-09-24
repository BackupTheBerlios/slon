#ifndef __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__
#define __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__

#include <boost/signal.hpp>
#include "../Thread/Timer.h"
#include "DynamicsWorld.h"

namespace slon {
namespace physics {

class SLON_PUBLIC PhysicsManager
{
public:
    typedef boost::signal<void ()>          pre_frame_signal;
    typedef boost::signal<void ()>          post_frame_signal;
    typedef boost::signals::connection      connection_type;

    typedef std::list<dynamics_world_ptr>   dynamics_world_list;
    typedef dynamics_world_list::iterator   dynamics_world_iterator;

public:
    /** Add dynamics world to physics manager. */
    virtual void addDynamicsWorld(const dynamics_world_ptr& world) = 0;

    /** Remove dynamics world from physics manager. */
    virtual void removeDynamicsWorld(const dynamics_world_ptr& world) = 0;

    /** Get iterator pointing first dynamics world. */
    virtual dynamics_world_iterator firstDynamicsWorld() = 0;

    /** Get iterator pointing after last dynamics world. */
    virtual dynamics_world_iterator endDynamicsWorld() = 0;

    /** Set timer for physics simulation. If timer is 0 simulation will stop. */
    virtual void setTimer(const const_timer_ptr& timer) = 0;

    /** Get physics timer */
    virtual const Timer* getTimer() const = 0;

    /** Attach pre frame callback. Calls before physics frame simulation. */
    virtual connection_type connectPreFrameCallback(pre_frame_signal::slot_type slot) = 0;

    /** Attach post frame callback. Calls after physics frame simulation. */
    virtual connection_type connectPostFrameCallback(post_frame_signal::slot_type slot) = 0;

    virtual ~PhysicsManager() {}
};

/** Get current physics manager */
SLON_PUBLIC PhysicsManager& currentPhysicsManager();

} // namespace slon
} // namespace physics

#endif // __SLON_ENGINE_PHYSICS_PHYSICS_MANAGER_H__
