#ifndef __SLON_ENGINE_DATABASE_LIBRARY_H__
#define __SLON_ENGINE_DATABASE_LIBRARY_H__

#include "../Realm/Location.h"
#include "../Realm/Object.h"
#include "Forward.h"
#include <boost/unordered_map.hpp>

namespace slon {
namespace database {

/** Interface for library loaders */
class Library :
    public Referenced
{
public:
	template<typename T>
	struct element_type { typedef boost::intrusive_ptr<T> type; };

    template<typename T>
    struct storage_type { typedef boost::unordered_map<std::string, typename element_type<T>::type> type; };

	typedef storage_type<animation::Animation>::type::value_type   key_animation_pair;
    typedef storage_type<graphics::Effect>::type::value_type       key_effect_pair;
    typedef storage_type<graphics::Texture>::type::value_type      key_texture_pair;
    typedef storage_type<scene::Node>::type::value_type            key_visual_scene_pair;
    typedef storage_type<realm::Object>::type::value_type          key_object_pair;
    typedef storage_type<realm::Location>::type::value_type        key_location_pair;
#ifdef SLON_ENGINE_USE_PHYSICS
    typedef storage_type<physics::PhysicsModel>::type::value_type  key_physics_scene_pair;
#endif

	typedef storage_type<animation::Animation>::type   key_animation_map;
    typedef storage_type<graphics::Effect>::type       key_effect_map;
    typedef storage_type<graphics::Texture>::type      key_texture_map;
    typedef storage_type<scene::Node>::type            key_visual_scene_map;
    typedef storage_type<realm::Object>::type          key_object_map;
    typedef storage_type<realm::Location>::type        key_location_map;
#ifdef SLON_ENGINE_USE_PHYSICS
    typedef storage_type<physics::PhysicsModel>::type  key_physics_scene_map;
#endif
	
public:
    key_animation_map     animations;
    key_effect_map        effects;
    key_texture_map       textures;
    key_visual_scene_map  visualScenes;
    key_object_map        objects;
    key_location_map      locations;
#ifdef SLON_ENGINE_USE_PHYSICS
    key_physics_scene_map physicsScenes;
#endif
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_LIBRARY_H__
