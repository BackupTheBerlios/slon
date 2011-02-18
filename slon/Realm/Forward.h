#ifndef __SLON_ENGINE_REALM_FORWARD_H__
#define __SLON_ENGINE_REALM_FORWARD_H__

namespace boost {
	template<typename T> class intrusive_ptr;
}

namespace slon {
namespace realm {

// forward types
class Object;
class Location;
class World;

// ptr typedefs
typedef boost::intrusive_ptr<Object>			object_ptr;
typedef boost::intrusive_ptr<const Object>		const_object_ptr;
typedef boost::intrusive_ptr<Location>          location_ptr;
typedef boost::intrusive_ptr<const Location>    const_location_ptr;
typedef boost::intrusive_ptr<World>				world_ptr;
typedef boost::intrusive_ptr<const World>		const_world_ptr;

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_FORWARD_H__