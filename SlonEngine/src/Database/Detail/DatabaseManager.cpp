#include "stdafx.h"
#include "Detail/Engine.h"

DECLARE_AUTO_LOGGER("database.DatabaseManager")

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
        typedef typename database::Library::storage_type<T>::type::iterator iterator;
        for (iterator it = container.begin(); it != container.end(); ++it)
        {
            std::string key = keyPrefix + it->first;
            if ( !cache.find(key) ) {
                cache.add(key, it->second);
            }
            else if ( !ignoreDuplicates ) {
                throw loader_error(AUTO_LOGGER, "Duplicate item in the cache: " + key);
            }
            else {
                AUTO_LOGGER_MESSAGE(log::S_WARNING, "Duplicate item in the cache: " << key << LOG_FILE_AND_LINE); 
            }
        }
    }

} // anonymous namespace
   
DatabaseManager::DatabaseManager()
:   libraryCache(AUTO_LOGGER)
,   animationCache(AUTO_LOGGER)
,   effectCache(AUTO_LOGGER)
,   textureCache(AUTO_LOGGER)
,   visualSceneCache(AUTO_LOGGER)
#ifdef SLON_ENGINE_USE_PHYSICS
,   physicsSceneCache(AUTO_LOGGER)
#endif
{
}

DatabaseManager::~DatabaseManager()
{
}

DatabaseManager::format_desc* DatabaseManager::unwrap(format_id format) const
{
    if (format.pObj)
    {
        format_desc* fdesc = reinterpret_cast<format_desc*>(format.pObj);
        return fdesc;
    }

    return 0;
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
    addObjects(effectCache, library.effects, keyPrefix, ignoreDuplicates);
    addObjects(textureCache, library.textures, keyPrefix, ignoreDuplicates);
    addObjects(visualSceneCache, library.visualScenes, keyPrefix, ignoreDuplicates);
#ifdef SLON_ENGINE_USE_PHYSICS
    addObjects(physicsSceneCache, library.physicsScenes, keyPrefix, ignoreDuplicates);
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

Serializable* DatabaseManager::createSerializableByName(const std::string& name)
{
	// search for create func
	serializable_create_func_map::iterator it = serializableCreateFuncs.find(name);
	if ( it == serializableCreateFuncs.end() ) {
		return 0;
	}

	return (it->second)(); // call create func
}

bool DatabaseManager::registerSerializableCreateFunc(const std::string&				 name, 
													 const serializable_create_func& func)
{
	return serializableCreateFuncs.insert( std::make_pair(name, func) ).second;
}

bool DatabaseManager::unregisterSerializableCreateFunc(const std::string& name)
{
	return serializableCreateFuncs.erase(name) > 0;
}

} // namespace detail

DatabaseManager& currentDatabaseManager() 
{ 
    return Engine::Instance()->getDatabaseManager(); 
}

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

bool saveLibrary(const std::string& path,
				 const library_ptr& library,
                 format_id          format)
{
	return static_cast<detail::DatabaseManager&>(currentDatabaseManager()).getLibraryCache().save(path, library, format);
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
