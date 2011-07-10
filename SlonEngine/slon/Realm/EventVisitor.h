#ifndef __SLON_ENGINE_REALM_EVENT_VISITOR_H__
#define __SLON_ENGINE_REALM_EVENT_VISITOR_H__

#include "../Scene/Node.h"
#include "../Scene/Visitor/VisitorImpl.hpp"
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
    EventVisitor(TYPE type_, const World* world_, const Location* location_)
    :   type(type_)
    ,   world(world_)
    ,   location(location_)
    {}

    EventVisitor(TYPE type_, const World* world_, const Location* location_, scene::Node& node)
    :   type(type_)
    ,   world(world_)
    ,   location(location_)
    {
        traverse(node);
    }

    /** Get type of the event. */
    TYPE getType() const { return type; }

    /** Get world assigned to visitor. */
    const World* getWorld() const { return world; }

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
