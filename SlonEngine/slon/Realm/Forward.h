#ifndef __SLON_ENGINE_REALM_FORWARD_H__
#define __SLON_ENGINE_REALM_FORWARD_H__

#include "../Config.h"

namespace boost {
	template<typename T> class intrusive_ptr;
}

namespace slon {

template<typename Signature>
class callback;

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

typedef callback<bool (realm::Object&)>			object_callback;
typedef callback<bool (const realm::Object&)>	object_const_callback;

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_FORWARD_H__