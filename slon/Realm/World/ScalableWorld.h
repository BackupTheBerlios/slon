#ifndef __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__

#include "../Location.h"
#include "WorldBase.h"
#include <vector>

namespace slon {
namespace realm {

/** Very simple world. Stores array of locations and updates all
 * of them every frame. 
 */
class ScalableWorld :
    public WorldBase
{
public:
    typedef std::vector<location_ptr>   location_vector;
    typedef std::vector<object_ptr>     object_vector;

public:
    ScalableWorld();

    /** Add location to the world */
    void addLocation(Location* location);

    /** Remove location from the world.
     * @return true - if succeeds, false if location not found
     */
    bool removeLocation(Location* location);

    /** Check wether world have specified location */
    bool haveLocation(Location* location) const;

    // Override World
    UPDATE_POLICY   getUpdatePolicy() const     { return UPDATE_ACTIVE; }
    EVICTION_POLICY getEvictionPolicy() const   { return NO_EVICTION; }
    LOAD_POLICY     getLoadPolicy() const       { return LOAD_ON_STARTUP; }
	
    void visit(const math::Ray3f& ray, scene::NodeVisitor& nv);
    void visit(const math::Ray3f& ray, scene::ConstNodeVisitor& nv) const;
    void visit(const math::AABBf& area, scene::NodeVisitor& nv);
    void visit(const math::AABBf& area, scene::ConstNodeVisitor& nv) const;
    void visit(const math::Frustumf& frustum, scene::NodeVisitor& nv);
    void visit(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const;

    bool update(Object* object);
    bool remove(Object* object);
    void add(Object* object);

private:
    location_vector     locations;

    // infinite object
    object_vector       infiniteObjects;
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__