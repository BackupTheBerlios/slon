#ifndef __SLON_ENGINE_DATABASE_DETAIL_LIBRARY_H__
#define __SLON_ENGINE_DATABASE_DETAIL_LIBRARY_H__

#include <sgl/Texture.h>
#include "../../Animation/Animation.h"
#include "../../Graphics/Effect.h"
#include "../../Scene/Node.h"
#include "../Library.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "../../Physics/PhysicsModel.h"
#endif

namespace slon {
namespace database {
namespace detail {

class Library :
    public database::Library
{
public:
    // Override Library
	key_animation_array		getAnimations() const		{ return animations; }
    key_effect_array        getEffects() const          { return effects; }
    key_texture_array       getTextures() const         { return textures; }
    key_visual_scene_array  getVisualScenes() const     { return visualScenes; }
#ifdef SLON_ENGINE_USE_PHYSICS
    key_physics_scene_array getPhysicsScenes() const    { return physicsScenes; }
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

typedef boost::intrusive_ptr<Library>		library_ptr;
typedef boost::intrusive_ptr<const Library>	const_library_ptr;

}
}
}

#endif // __SLON_ENGINE_DATABASE_DETAIL_LIBRARY_H__