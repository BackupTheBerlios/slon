#ifndef __SLON_ENGINE_DATABASE_DATABASE_H__
#define __SLON_ENGINE_DATABASE_DATABASE_H__

#include "Cache.h"
#include "Library.h"
#include "Loader.h"

namespace slon {
namespace database {

/** Interface for accessing common objects in the storage. */
class DatabaseManager :
    public Referenced
{
public:
    typedef std::vector<std::string>            string_array;
    typedef std::vector<format_id>              format_array;
    typedef std::vector<library_loader_ptr>     library_loader_array;
    typedef std::vector<library_saver_ptr>      library_saver_array;

    static const format_id						library_format_auto;

    enum
    {
		CLEAR_ANIMATION_CACHE_BIT		= 1 << 0,
        CLEAR_EFFECT_CACHE_BIT          = 1 << 1,
        CLEAR_TEXTURE_CACHE_BIT         = 1 << 2,
        CLEAR_VISUAL_SCENE_CACHE_BIT    = 1 << 3
    #ifdef SLON_ENGINE_USE_PHYSICS
        , CLEAR_PHYSICS_SCENE_CACHE_BIT	= 1 << 4
    #endif
        , CLEAR_ALL                     = 0xFFFF
    };
	
	typedef boost::function<Serializable* ()>	serializable_create_func;

public:
    /** Get animation cache */
    virtual AnimationCache& getAnimationCache() = 0;

    /** Get effect cache */
    virtual EffectCache& getEffectCache() = 0;

    /** Get texture cache */
    virtual TextureCache& getTextureCache() = 0;

    /** Get visual scene cache */
    virtual VisualSceneCache& getVisualSceneCache() = 0;

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Get physics scene cache */
    virtual PhysicsSceneCache& getPhysicsSceneCache() = 0;
#endif

    /** Remove objects from the storage. Removes all cached libraries as though.
     * @param mask - clear mask. Can be combination of the following:
     * \li \c CLEAR_EFFECT_CACHE_BIT
     * \li \c CLEAR_TEXTURE_CACHE_BIT
     * \li \c CLEAR_VISUAL_SCENE_CACHE_BIT
     * \li \c CLEAR_PHYSICS_SCENE_CACHE_BIT
     * \li \c CLEAR_ALL
     */
    virtual void clear(unsigned mask) = 0;

    /** Look for item in the cache. If not found then call loader and add item into storage.
     * @param path - library file path. Library loaders may interpret them in the way they want it.
     * @param keyPrefix - keyPrefix used to generate unique keys.
     * @param loader - library loader.
     * @param ignoreDuplicates - if true item with duplicate keys won't be added into the caches, otherwise
     * function will throw loader_error if it find item with duplicate key.
     */
    virtual library_ptr loadLibrary(const std::string&  path, 
                                    const std::string&  keyPrefix, 
                                    LibraryLoader&      loader,
                                    bool                ignoreDuplicates = true) = 0;

    /** Load library of resources into the storage. Function will try every loader specified for item format.
     * @param path - library file path. Library loaders may interpret them in the way they want it.
     * @param keyPrefix - keyPrefix used to generate unique keys.
     * @param format - library file format. If format == format_auto, function will try any format
     * returned by getAppropriateLibraryFormats(path) function.
     * @param ignoreDuplicates - if true item with duplicate keys won't be added into the caches, otherwise
     * function will throw loader_error if it find item with duplicate key.
     */
    virtual library_ptr loadLibrary(const std::string& path,
                                    const std::string& keyPrefix,
                                    format_id          format = library_format_auto,
                                    bool               ignoreDuplicates = true) = 0;

    /** Get all registered formats. */
    virtual format_array getLibraryFormats() const = 0;

    /** Get format list by the extension. Formats are sorted by matching probability, which
     * is determined using the order of regular expressions listed using registerFormat. The any expression ".*"
     * has lowest priority, no matter of its index.
     * For example:
     * \code
     * pathExpr = { ".*\\.[jJ][pP][eE]?[gG]", ".*" };
     * registerFormat( jpeg_format_id, pathExpr );
     * pathExpr = { ".*\\.(?i:bmp)" };
     * registerFormat( bmp_format_id, pathExpr );
     * pathExpr = { ".*\\.[iI][cC][oO]", ".*" };
     * registerFormat( ico_format_id, pathExpr );
     * pathExpr = { ".*" };
     * registerFormat( user_format_id, pathExpr );
     *
     * getAppropriateFormats("C:\Images\pic.jpg"); // returns { jpeg_format_id, bmp_format_id, "user_format_id" }
     * getAppropriateFormats("C:\Images\pic.");    // returns any combination of jpeg_format_id, bmp_format_id, user_format_id
     * \uncode
     * @param path - path to the file to check the extension.
     * @see registerFormat
     */
    virtual format_array getAppropriateLibraryFormats(const std::string& path) const = 0;

    /** Register library format. If format already present - replace it.
     * @param format - format identifier.
     * @param pathExpr - regular expressions for determining format using file name (e.g ".*\\.(?i:dae)", ".*"). 
     * List any expression - ".*" if format can be determined by the file source. Expressions are case sensitive.
     * Order of expressions in the array will be used to determine probability of matching format. Any expression ".*"
     * has the lowest priority.
     * @see getAppropriateFormats
     */
    virtual format_id registerLibraryFormat(string_array pathExpr) = 0;

    /** Unregister library format. Function will not remove format loaders. */
    virtual void unregisterLibraryFormat(format_id format) = 0;

    /** Remove all library formats from the cache. */
    virtual void clearLibraryFormats() = 0;

    /** Get all registered loaders. */
    virtual library_loader_array getLibraryLoaders() const = 0;
	
    /** Get all registered saver. */
    virtual library_saver_array getLibrarySavers() const = 0;

    /** Get loaders for the specified format. Order of loaders in the array is not specified. */
    virtual library_loader_array getAppropriateLibraryLoaders(format_id format) const = 0;

    /** Get savers for the specified format. Order of savers in the array is not specified. */
    virtual library_saver_array getAppropriateLibrarySavers(format_id format) const = 0;

    /** Register loader for specified format, replace existent if present.
     * @param format - format for wich loader is specified.
     * @param loader - new loader for provided format.
     */
    virtual void registerLibraryLoader(format_id format, const library_loader_ptr& loader) = 0;

    /** Unregister loader for specified format. 
     * @param format - format supported by loader.
     * @param loader - loader for removal.
     */
    virtual void unregisterLibraryLoader(format_id format, LibraryLoader* loader) = 0;

    /** Unregister loader for all formats.
     * @param loader - loader for removal.
     * @return number of formats for which loader was unregistered.
     */
    virtual size_t unregisterLibraryLoader(LibraryLoader* loader) = 0;

    /** Remove all library loaders from the cache */
    virtual void clearLibraryLoaders() = 0;
	
    /** Register saver for specified format, replace existent if present.
     * @param saver - format for which saver is specified.
     * @param saver - new saver for provided format.
     */
	virtual void registerLibrarySaver(format_id format, const library_saver_ptr& saver) = 0;
	
    /** Unregister saver for specified format. 
     * @param format - format supported by loader.
     * @param saver - saver for removal.
     */
	virtual void unregisterLibrarySaver(format_id format, LibrarySaver* saver) = 0;

    /** Unregister saver for all formats.
     * @param saver - saver for removal.
     * @return number of formats for which saver was unregistered.
     */
	virtual size_t unregisterLibrarySaver(LibrarySaver* saver) = 0;

    /** Remove all savers from the cache. */
    virtual void clearLibrarySavers() = 0;

	/** Create serializable using its name. */
	virtual Serializable* createSerializableByName(const std::string& name) = 0;

	/** Register serializable create func. Archive will use this function to create serializable using its name.
	 * @param name - serializable name.
	 * @param func - serializable create function.
	 * @param useWrapper - use serializable wrapper for object serialization/deserialization.
	 * @return true if succeeded, false if create func with specified name already exists.
	 */
	virtual bool registerSerializableCreateFunc(const std::string& name, const serializable_create_func& func) = 0;

	/** Unregister serializable create func. Remove serializable create function for specified serializable name. 
	 * @param name - serializable name.
	 * @return true if succeeded, false if name was not found.
	 */
	virtual bool unregisterSerializableCreateFunc(const std::string& name) = 0;

    virtual ~DatabaseManager() {}
};

/** Get engines database manager */
DatabaseManager& currentDatabaseManager();

/** Get current cache of the specified item */
template<typename T>
Cache<T>& currentCache();

/** Load library, add all loaded items into corresponding cache. 
 * @param path - path to the library file.
 * @param keyPrefix - prefix for item keys to avoid duplicates.
 * @param format - format of the library to load.
 * @param ignoreDuplicates - if true item with duplicate keys won't be added into the caches, otherwise
 * function will throw loader_error if it find item with duplicate key.
 * @return loaded library.
 */
library_ptr loadLibrary(const std::string& path,
                        const std::string& keyPrefix,
                        format_id          format = DatabaseManager::library_format_auto,
                        bool               ignoreDuplicates = true);

/** Load library, add all loaded items into corresponding cache. Use path as key prefix. 
 * @param path - path to the library file.
 * @param format - format of the library to load.
 * @param ignoreDuplicates - if true item with duplicate keys won't be added into the caches, otherwise
 * function will throw loader_error if it find item with duplicate key.
 * @return loaded library.
 */
library_ptr loadLibrary(const std::string& path,
                        format_id          format = DatabaseManager::library_format_auto,
                        bool               ignoreDuplicates = true);

/** Save library into file. 
 * @param path - path to the library file.
 * @param library - library to save.
 * @param format - format of the library to save.
 * @return true on success.
 */
bool saveLibrary(const std::string& path,
				 const library_ptr& library,
                 format_id          format = DatabaseManager::library_format_auto);

/** Look for texture in the cache, if not found, load it from file and add to cache. 
 * @param path - path to the image file.
 * @param format - format of the image to load.
 * @return loaded texture.
 */
graphics::texture_ptr loadTexture(const std::string& path,
                                  format_id          format = TextureCache::format_auto);

/** Save texture to the file.
 * @param path - path to the image file.
 * @param texture - texture to save.
 * @param format - format of the image to save.
 * @return true on success.
 */
bool saveTexture(const std::string&				path,
				 const graphics::texture_ptr&	texture,
                 format_id						format = TextureCache::format_auto);

/** Look for effect in the cache, if not found, load it from file and add to cache. 
 * @param path - path to the effect file.
 * @param format - format of the image to load.
 * @return loaded effect.
 */
graphics::effect_ptr loadEffect(const std::string& path,
                                format_id          format = EffectCache::format_auto);

/** Save effect to the file.
 * @param path - path to the effect file.
 * @param effect - effect to save.
 * @param format - format of the effect to save.
 * @return true on success.
 */
bool saveEffect(const std::string&			path,
				const graphics::effect_ptr&	effect,
                format_id					format = EffectCache::format_auto);

/** Look for visual scene in the cache, if not found, load it from file and add to cache. 
 * @param path - path to the scene file.
 * @param format - format of the image to load.
 * @return loaded scene.
 */
scene::node_ptr loadVisualScene(const std::string& path,
                                format_id          format = VisualSceneCache::format_auto);

/** Save visual scene to the file.
 * @param path - path to the visual scene file.
 * @param scene - visual scene to save.
 * @param format - format of the visual scene to save.
 * @return true on success.
 */
bool saveVisualScene(const std::string&			path,
					 const scene::node_ptr&		scene,
					 format_id					format = VisualSceneCache::format_auto);

#ifdef SLON_ENGINE_USE_PHYSICS
/** Look for physics scene in the cache, if not found, load it from file and add to cache. 
 * @param path - path to the scene file.
 * @param format - format of the image to load.
 * @return loaded scene.
 */
physics::physics_model_ptr loadPhysicsScene(const std::string& path,
                                            format_id          format = PhysicsSceneCache::format_auto);

/** Save physics scene to the file.
 * @param path - path to the visual scene file.
 * @param scene - physics scene to save.
 * @param format - format of the physics scene to save.
 * @return true on success.
 */
bool savePhysicsScene(const std::string&				path,
					  const physics::physics_model_ptr&	scene,
					  format_id							format = PhysicsSceneCache::format_auto);

#endif

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_DATABASE_H__
