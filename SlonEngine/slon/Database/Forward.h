#ifndef __SLON_ENGINE_DATABASE_FORWARD_H__
#define __SLON_ENGINE_DATABASE_FORWARD_H__

#include "../Animation/Forward.h"
#include "../Graphics/Forward.h"
#include "../Realm/Forward.h"
#include "../Scene/Forward.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#include "../Physics/Forward.h"
#endif

namespace slon {
namespace database {

// forward types;
template<typename T> class Cache;
template<typename T> class Loader;
template<typename T> class Saver;

class DatabaseManager;
class Library;
class Archive;
class IArchive;
class OArchive;
class Serializable;

typedef boost::intrusive_ptr<Library>               library_ptr;
typedef boost::intrusive_ptr<const Library>         const_library_ptr;
typedef boost::intrusive_ptr<Archive>               archive_ptr;
typedef boost::intrusive_ptr<const Archive>         const_archive_ptr;
typedef boost::intrusive_ptr<IArchive>              iarchive_ptr;
typedef boost::intrusive_ptr<const IArchive>        const_iarchive_ptr;
typedef boost::intrusive_ptr<OArchive>              oarchive_ptr;
typedef boost::intrusive_ptr<const OArchive>        const_oarchive_ptr;

// common typedefs
typedef Loader<library_ptr>                     LibraryLoader;
typedef Loader<animation::animation_ptr>        AnimationLoader;
typedef Loader<graphics::effect_ptr>            EffectLoader;
typedef Loader<graphics::texture_ptr>           TextureLoader;
typedef Loader<scene::node_ptr>                 VisualSceneLoader;
typedef Loader<realm::object_ptr>               ObjectLoader;
typedef Loader<realm::location_ptr>             LocationLoader;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef Loader<physics::physics_model_ptr>      PhysicsSceneLoader;
#endif

typedef Saver<library_ptr>                      LibrarySaver;
typedef Saver<animation::animation_ptr>         AnimationSaver;
typedef Saver<graphics::effect_ptr>             EffectSaver;
typedef Saver<graphics::texture_ptr>            TextureSaver;
typedef Saver<scene::node_ptr>                  VisualSceneSaver;
typedef Saver<realm::object_ptr>                ObjectSaver;
typedef Saver<realm::location_ptr>              LocationSaver;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef Saver<physics::physics_model_ptr>       PhysicsSceneSaver;
#endif

typedef Cache<animation::Animation>             AnimationCache;
typedef Cache<graphics::Effect>                 EffectCache;
typedef Cache<graphics::Texture>                TextureCache;
typedef Cache<scene::Node>                      VisualSceneCache;
typedef Cache<realm::Object>                    ObjectCache;
typedef Cache<realm::Location>                  LocationCache;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef Cache<physics::PhysicsModel>            PhysicsSceneCache;
#endif

// common ptr typedefs
typedef boost::intrusive_ptr<AnimationLoader>           animation_loader_ptr;
typedef boost::intrusive_ptr<const AnimationLoader>     const_animation_loader_ptr;
typedef boost::intrusive_ptr<LibraryLoader>             library_loader_ptr;
typedef boost::intrusive_ptr<const LibraryLoader>       const_library_loader_ptr;
typedef boost::intrusive_ptr<EffectLoader>              effect_loader_ptr;
typedef boost::intrusive_ptr<const EffectLoader>        const_effect_loader_ptr;
typedef boost::intrusive_ptr<TextureLoader>             texture_loader_ptr;
typedef boost::intrusive_ptr<const TextureLoader>       const_texture_loader_ptr;
typedef boost::intrusive_ptr<VisualSceneLoader>         visual_scene_loader_ptr;
typedef boost::intrusive_ptr<const VisualSceneLoader>   const_visual_scene_loader_ptr;
typedef boost::intrusive_ptr<ObjectLoader>              object_loader_ptr;
typedef boost::intrusive_ptr<const ObjectLoader>        const_object_loader_ptr;
typedef boost::intrusive_ptr<LocationLoader>            location_loader_ptr;
typedef boost::intrusive_ptr<const LocationLoader>      const_location_loader_ptr;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef boost::intrusive_ptr<PhysicsSceneLoader>        physics_scene_loader_ptr;
typedef boost::intrusive_ptr<const PhysicsSceneLoader>  const_physics_scene_loader_ptr;
#endif

typedef boost::intrusive_ptr<AnimationSaver>            animation_saver_ptr;
typedef boost::intrusive_ptr<const AnimationSaver>      const_animation_saver_ptr;
typedef boost::intrusive_ptr<LibrarySaver>              library_saver_ptr;
typedef boost::intrusive_ptr<const LibrarySaver>        const_library_saver_ptr;
typedef boost::intrusive_ptr<EffectSaver>               effect_saver_ptr;
typedef boost::intrusive_ptr<const EffectSaver>         const_effect_saver_ptr;
typedef boost::intrusive_ptr<TextureSaver>              texture_saver_ptr;
typedef boost::intrusive_ptr<const TextureSaver>        const_texture_saver_ptr;
typedef boost::intrusive_ptr<VisualSceneSaver>          visual_scene_saver_ptr;
typedef boost::intrusive_ptr<const VisualSceneSaver>    const_visual_scene_saver_ptr;
typedef boost::intrusive_ptr<ObjectSaver>               object_saver_ptr;
typedef boost::intrusive_ptr<const ObjectSaver>         const_object_saver_ptr;
typedef boost::intrusive_ptr<LocationSaver>             location_saver_ptr;
typedef boost::intrusive_ptr<const LocationSaver>       const_location_saver_ptr;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef boost::intrusive_ptr<PhysicsSceneSaver>         physics_scene_saver_ptr;
typedef boost::intrusive_ptr<const PhysicsSceneSaver>   const_physics_scene_saver_ptr;
#endif

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_FORWARD_H__