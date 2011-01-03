#ifndef __SLON_ENGINE_DATABASE_LIBRARY_H__
#define __SLON_ENGINE_DATABASE_LIBRARY_H__

#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <vector>

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
    /** Enumerate loaded effects */
    virtual key_animation_array getAnimations() const = 0;

    /** Enumerate loaded effects */
    virtual key_effect_array getEffects() const = 0;

    /** Enumerate loaded textures */
    virtual key_texture_array getTextures() const = 0;

    /** Enumerate loaded visual scenes */
    virtual key_visual_scene_array getVisualScenes() const = 0;

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Enumerate loaded physics scenes */
    virtual key_physics_scene_array getPhysicsScenes() const = 0;
#endif

    virtual ~Library() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_LIBRARY_H__
