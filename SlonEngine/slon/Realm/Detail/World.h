#ifndef __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__
#define __SLON_ENGINE_REALM_WORLD_SCALABLE_WORLD_H__

#include <boost/thread/shared_mutex.hpp>
#include <vector>
#include "../Location.h"
#include "../World.h"

namespace slon {
namespace realm {
namespace detail {

/** Very simple world. Stores array of locations and updates all
 * of them every frame. 
 */
class World :
	public realm::World
{
public:
    typedef std::vector<location_ptr>		location_vector;
    typedef std::vector<scene::node_ptr>	object_vector;

public:
    World();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

	template<typename Body>
	void visit(const Body& body, scene::NodeVisitor& nv)
	{
		// visit infinite objects
		for (size_t i = 0; i<infiniteObjects.size(); ++i) {
			nv.traverse(*infiniteObjects[i]);
		}

		// visit others
		for (size_t i = 0; i<locations.size(); ++i)
		{
			if ( test_intersection(locations[i]->getBounds(), body) ) {
				locations[i]->visit(body, nv);
			}
		}
	}

	template<typename Body>
	void visit(const Body& body, scene::ConstNodeVisitor& nv) const
	{
		// visit infinite objects
		for (size_t i = 0; i<infiniteObjects.size(); ++i) {
			nv.traverse(*infiniteObjects[i]);
		}

		// visit others
		for (size_t i = 0; i<locations.size(); ++i)
		{
			if ( test_intersection(locations[i]->getBounds(), body) ) {
				locations[i]->visit(body, nv);
			}
		}
	}
	
    // Override World
    void addLocation(Location* location);
    bool removeLocation(Location* location);
    bool haveLocation(Location* location) const;
		
    void visit(const body_variant& body, scene::NodeVisitor& nv);
    void visit(const body_variant& body, scene::ConstNodeVisitor& nv) const;
    void visitVisible(const math::Frustumf& frustum, scene::NodeVisitor& nv);
    void visitVisible(const math::Frustumf& frustum, scene::ConstNodeVisitor& nv) const;

    bool removeInfiniteNode(const scene::node_ptr& node);
    void addInfiniteNode(const scene::node_ptr& node);
	bool haveInfiniteNode(const scene::node_ptr& node) const;
	
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
