#ifndef __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__

#include <boost/thread/shared_mutex.hpp>
#include <vector>
#include "../Location.h"
#include "../World.h"
#include "Object.h"

namespace slon {
namespace realm {
namespace detail {

/** Very simple world. Stores array of locations and updates all
 * of them every frame. 
 */
class World :
	public realm::World
{
template<typename World, typename Callback>
friend class WorldVisitor;
public:
    typedef std::vector<location_ptr>   location_vector;
    typedef std::vector<object_ptr>     object_vector;

public:
    World();

    /** Add location to the world */
    void addLocation(Location* location);

    /** Remove location from the world.
     * @return true - if succeeds, false if location not found
     */
    bool removeLocation(Location* location);

    /** Check whether world have specified location */
    bool haveLocation(Location* location) const;
		
	template<typename Body>
	void visit(const Body& body, object_callback& cb)
	{
		// visit infinite objects
		for (size_t i = 0; i<infiniteObjects.size(); ++i) 
		{
			if ( cb(*infiniteObjects[i]) ) {
				return;
			}
		}

		// visit others
		for (size_t i = 0; i<locations.size(); ++i)
		{
			if ( test_intersection(locations[i]->getBounds(), body) ) {
				locations[i]->visit(body, cb);
			}
		}
	}

	template<typename Body>
	void visit(const Body& body, object_const_callback& cb) const
	{
		// visit infinite objects
		for (size_t i = 0; i<infiniteObjects.size(); ++i) 
		{
			if ( cb(*infiniteObjects[i]) ) {
				return;
			}
		}

		// visit others
		for (size_t i = 0; i<locations.size(); ++i)
		{
			if ( test_intersection(locations[i]->getBounds(), body) ) {
				locations[i]->visit(body, cb);
			}
		}
	}

    void visit(const body_variant& body, object_callback& cb);
    void visit(const body_variant& body, object_const_callback& cb) const;
    void visitVisible(const math::Frustumf& frustum, object_callback& cb);
    void visitVisible(const math::Frustumf& frustum, object_const_callback& cb) const;

	realm::Object*  createObject() const;

    bool			update(realm::Object* object);
    bool			remove(realm::Object* object);
    void			add(realm::Object* object);
	realm::Object*	add(scene::Node*				node,
						bool						dynamic
#ifdef SLON_ENGINE_USE_PHYSICS
						, physics::PhysicsModel*	physicsModel
#endif                  
						);
	
    thread::lock_ptr lockForReading() const;
    thread::lock_ptr lockForWriting();

private:
    location_vector locations;

    // infinite object
    object_vector	infiniteObjects;

    mutable boost::shared_mutex accessMutex;
};

} // namespace detail
} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__
