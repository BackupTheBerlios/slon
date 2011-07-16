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
        WORLD_ADD,      /// node added into the world
        WORLD_REMOVE    /// node removed from the world
    };

public:
    EventVisitor()
    :   world(0)
    ,   location(0)
    {}

    EventVisitor(TYPE type_, const Location* location_)
    :   type(type_)
    ,   world(0)
    ,   location(location_)
    {}

    EventVisitor(TYPE type_, const World* world_)
    :   type(type_)
    ,   world(world_)
    ,   location(0)
    {}

    /** Set event type. */
    void setType(TYPE type_) { type = type_; }

    /** Get type of the event. */
    TYPE getType() const { return type; }

    /** Set visitor world. */
    void setWorld(World* world_) { world = world_; }

    /** Get world assigned to visitor. */
    const World* getWorld() const { return world; }

    /** Set visitor location. */
    void setLocation(Location* location_) { location = location_; }

    /** Get location assigned to visitor. */
    const Location* getLocation() const { return location; }

private:
    TYPE            type;
    const World*    world;
    const Location* location;
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_EVENT_VISITOR_H__
