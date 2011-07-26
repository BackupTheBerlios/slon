#ifndef __SLON_ENGINE_REALM_EVENT_VISITOR_H__
#define __SLON_ENGINE_REALM_EVENT_VISITOR_H__

#include "../Scene/Node.h"
#include "../Scene/VisitorImpl.hpp"
#include "Forward.h"

namespace slon {
namespace realm {

/** Event visitor notifies scene graph nodes about being added/removed to/form world.
 */
class EventVisitor :
    public scene::VisitorImpl<EventVisitor, scene::Visitor>
{
public:
    enum TYPE
    {
        UNDEFINED,      /// for debug
        WORLD_ADD,      /// node added into the world
        WORLD_REMOVE    /// node removed from the world
    };

public:
    EventVisitor()
    :   type(UNDEFINED)
    ,   togglePhysics(false)
    ,   world(0)
    ,   location(0)
    {}

    EventVisitor(TYPE type_, Location* location_)
    :   type(type_)
    ,   togglePhysics(false)
    ,   world(0)
    ,   location(location_)
    {}

    EventVisitor(TYPE type_, World* world_)
    :   type(type_)
    ,   togglePhysics(false)
    ,   world(world_)
    ,   location(0)
    {}

    /** Set event type. */
    void setType(TYPE type_) { type = type_; }

    /** Get type of the event. */
    TYPE getType() const { return type; }

    /** Set flag for notifying physics objects. */
    void setPhysicsToggle(bool togglePhysics_) { togglePhysics = togglePhysics_; }

    /** Get flag for notifying physics objects. */
    bool getPhysicsToggle() const { return togglePhysics; }

    /** Set visitor world. */
    void setWorld(World* world_) { world = world_; }

    /** Get world assigned to visitor. */
    World* getWorld() { return world; }

    /** Set visitor location. */
    void setLocation(Location* location_) { location = location_; }
	
    /** Get location assigned to visitor. */
    Location* getLocation() { return location; }

private:
    TYPE      type;
    bool      togglePhysics;
    World*    world;
    Location* location;
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_EVENT_VISITOR_H__
