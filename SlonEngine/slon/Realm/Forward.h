#ifndef __SLON_ENGINE_REALM_FORWARD_H__
#define __SLON_ENGINE_REALM_FORWARD_H__

#include "../Config.h"

namespace boost {
	template<typename T> class intrusive_ptr;
}

namespace slon {
namespace realm {

// forward types
class Location;
class BVHLocation;
class BVHLocationNode;
class World;

// ptr typedefs
typedef boost::intrusive_ptr<Location>              location_ptr;
typedef boost::intrusive_ptr<const Location>        const_location_ptr;
typedef boost::intrusive_ptr<BVHLocation>           bvh_location_ptr;
typedef boost::intrusive_ptr<const BVHLocation>     const_bvh_location_ptr;
typedef boost::intrusive_ptr<BVHLocationNode>       bvh_location_node_ptr;
typedef boost::intrusive_ptr<const BVHLocationNode> const_bvh_location_node_ptr;
typedef boost::intrusive_ptr<World>				    world_ptr;
typedef boost::intrusive_ptr<const World>		    const_world_ptr;

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_FORWARD_H__