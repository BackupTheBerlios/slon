#include "stdafx.h"
#include "Detail/Engine.h"

__DEFINE_LOGGER__("database.DatabaseManager")

namespace slon {
namespace database {

const format_id DatabaseManager::library_format_auto = format_id(0);
    
namespace detail {

namespace {

    template<typename T>
    void addObjects(Cache<T>&                                         cache,
                    typename database::Library::storage_type<T>::type container,
                    const std::string&                                keyPrefix,
                    bool                                              ignoreDuplicates)
    {
        for (size_t i = 0; i<container.size(); ++i)
        {
            std::string key = keyPrefix + container[i].first;
            if ( !cache.find(key) ) {
                cache.add(key, container[i].second);
            }
            else if ( !ignoreDuplicates ) {
                throw loader_error(logger, "Duplicate item in the cache: " + key);
            }
            else {
                logger << log::S_WARNING << "Duplicate item in the cache: " << key << LOG_FILE_AND_LINE; 
            }
        }
    }

} // anonymous namespace
   
DatabaseManager::DatabaseManager()
:   libraryCache(&logger)
,   animationCache(&logger)
,   effectCache(&logger)
,   textureCache(&logger)
,   visualSceneCache(&logger)
#ifdef SLON_ENGINE_USE_PHYSICS
,   physicsSceneCache(&logger)
#endif
{
}

DatabaseManager::format_desc DatabaseManager::makeFormatDesc(format_id id, const string_array& regexps)
{
    format_desc desc;
    desc.id = id;
    desc.haveAnyMatch = false;
    
    boost::xpressive::sregex_compiler compiler;
    for (size_t i = 0; i<regexps.size(); ++i)
    {
        if (regexps[i] == "*") {
            desc.haveAnyMatch = true;
        }
        else {
            desc.pathExprs.push_back( compiler.compile(regexps[i]) );
        }
    }

    return desc;
}

void DatabaseManager::addLibraryObjects(const database::Library& library,
                                        const std::string&       keyPrefix,
                                        bool                     ignoreDuplicates)
{
    addObjects(effectCache, library.getEffects(), keyPrefix, ignoreDuplicates);
    addObjects(textureCache, library.getTextures(), keyPrefix, ignoreDuplicates);
    addObjects(visualSceneCache, library.getVisualScenes(), keyPrefix, ignoreDuplicates);
#ifdef SLON_ENGINE_USE_PHYSICS
    addObjects(physicsSceneCache, library.getPhysicsScenes(), keyPrefix, ignoreDuplicates);
#endif
}

void DatabaseManager::clear(unsigned mask)
{
    if (mask != 0) {
        libraryCache.clear();
    }
    if (mask & CLEAR_EFFECT_CACHE_BIT) {
        effectCache.clear();
    }
    if (mask & CLEAR_TEXTURE_CACHE_BIT) {
        textureCache.clear();
    }
    if (mask & CLEAR_VISUAL_SCENE_CACHE_BIT) {
        visualSceneCache.clear();
    }
#ifdef SLON_ENGINE_USE_PHYSICS
    if (mask & CLEAR_PHYSICS_SCENE_CACHE_BIT) {
        physicsSceneCache.clear();
    }
#endif
}

library_ptr DatabaseManager::loadLibrary(const std::string& path, 
                                         const std::string& keyPrefix, 
                                         LibraryLoader&     loader,
                                         bool               ignoreDuplicates)
{
    library_ptr library = libraryCache.load(path, keyPrefix, loader);
    if (library) {
        addLibraryObjects(*library, keyPrefix, ignoreDuplicates);
    }

    return library;
}

library_ptr DatabaseManager::loadLibrary(const std::string& path,
                                         const std::string& keyPrefix,
                                         format_id          format,
                                         bool               ignoreDuplicates)
{
    library_ptr library = libraryCache.load(path, keyPrefix, format);
    if (library) {
        addLibraryObjects(*library, keyPrefix, ignoreDuplicates);
    }

    return library;
}

} // namespace detail

DatabaseManager& currentDatabaseManager() 
{ 
    return Engine::Instance()->getDatabaseManager(); 
}

template<> Cache<graphics::Effect>&         currentCache() { return currentDatabaseManager().getEffectCache(); }
template<> Cache<graphics::Texture>&        currentCache() { return currentDatabaseManager().getTextureCache(); }
template<> Cache<scene::Node>&              currentCache() { return currentDatabaseManager().getVisualSceneCache(); }
#ifdef SLON_ENGINE_USE_PHYSICS
template<> Cache<physics::PhysicsModel>&    currentCache() { return currentDatabaseManager().getPhysicsSceneCache(); }
#endif

library_ptr loadLibrary(const std::string& path,
                        const std::string& keyPrefix,
                        format_id          format,
                        bool               ignoreDuplicates)
{
    return currentDatabaseManager().loadLibrary(path, keyPrefix, format, ignoreDuplicates);
}

library_ptr loadLibrary(const std::string& path,
                        format_id          format,
                        bool               ignoreDuplicates)
{
    return currentDatabaseManager().loadLibrary(path, path, format, ignoreDuplicates);
}

graphics::texture_ptr loadTexture(const std::string& path,
                                  format_id          format)
{
    return currentDatabaseManager().getTextureCache().load(path, format);
}

graphics::effect_ptr loadEffect(const std::string& path,
                                format_id          format)
{
    return currentDatabaseManager().getEffectCache().load(path, format);
}

scene::node_ptr loadVisualScene(const std::string& path,
                                format_id          format)
{
    return currentDatabaseManager().getVisualSceneCache().load(path, format);
}

#ifdef SLON_ENGINE_USE_PHYSICS

physics::physics_model_ptr loadPhysicsScene(const std::string& path,
                                            format_id          format)
{
    return currentDatabaseManager().getPhysicsSceneCache().load(path, format);
}

bool savePhysicsScene(const std::string&				path,
					  const physics::physics_model_ptr&	scene,
					  format_id							format)
{
    return currentDatabaseManager().getPhysicsSceneCache().save(path, scene, format);
}

#endif // SLON_ENGINE_USE_PHYSICS

} // namespace database
} // namespace slon
