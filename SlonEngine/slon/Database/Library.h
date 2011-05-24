#ifndef __SLON_ENGINE_DATABASE_LIBRARY_H__
#define __SLON_ENGINE_DATABASE_LIBRARY_H__

#include <vector>
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace database {

/** Interface for library loaders */
class Library :
    public Referenced
{
public:
    template<typename T>
    struct element_type { typedef std::pair<std::string, boost::intrusive_ptr<T> > type; };

    template<typename T>
    struct storage_type { typedef std::vector<typename element_type<T>::type> type; };

	typedef element_type<animation::Animation>::type   key_animation_pair;
    typedef element_type<graphics::Effect>::type       key_effect_pair;
    typedef element_type<graphics::Texture>::type      key_texture_pair;
    typedef element_type<scene::Node>::type            key_visual_scene_pair;
#ifdef SLON_ENGINE_USE_PHYSICS
    typedef element_type<physics::PhysicsModel>::type  key_physics_scene_pair;
#endif

	typedef storage_type<animation::Animation>::type   key_animation_array;
    typedef storage_type<graphics::Effect>::type       key_effect_array;
    typedef storage_type<graphics::Texture>::type      key_texture_array;
    typedef storage_type<scene::Node>::type            key_visual_scene_array;
#ifdef SLON_ENGINE_USE_PHYSICS
    typedef storage_type<physics::PhysicsModel>::type  key_physics_scene_array;
#endif
	
public:
	key_animation_array		animations;
    key_effect_array        effects;
    key_texture_array       textures;
    key_visual_scene_array  visualScenes;
#ifdef SLON_ENGINE_USE_PHYSICS
    key_physics_scene_array physicsScenes;
#endif
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_LIBRARY_H__
