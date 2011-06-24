#ifndef __SLON_ENGINE_DATABASE_DETAIL_DATABASE_MANAGER_H__
#define __SLON_ENGINE_DATABASE_DETAIL_DATABASE_MANAGER_H__

#include "../DatabaseManager.h"
#include "Cache.h"

namespace slon {
namespace database {
namespace detail {
	
typedef Cache<Library>                  LibraryCache;
typedef Cache<animation::Animation>     AnimationCache;
typedef Cache<graphics::Effect>         EffectCache;
typedef Cache<graphics::Texture>        TextureCache;
typedef Cache<scene::Node>              VisualSceneCache;
typedef Cache<realm::Object>            ObjectCache;
typedef Cache<realm::Location>          LocationCache;
#ifdef SLON_ENGINE_USE_PHYSICS
typedef Cache<physics::PhysicsModel>    PhysicsSceneCache;
#endif

class DatabaseManager :
    public database::DatabaseManager
{
private:
#if BOOST_VERSION < 104200 && __GNUC__
    typedef sgl::vector<boost::xpressive::sregex>   regex_vector;
#else
    typedef std::vector<boost::xpressive::sregex>   regex_vector;
#endif

    struct format_desc
    {
        format_id       id;
        regex_vector    pathExprs;
        bool            haveAnyMatch;
    };

    typedef std::vector<format_desc>                    format_desc_vector;
    typedef std::map<format_id, library_loader_array>   format_loader_map;
    typedef std::map<format_id, library_saver_array>    format_saver_map;

	typedef boost::unordered_map<std::string, serializable_create_func>	serializable_create_func_map;

private:
    format_desc makeFormatDesc(format_id            id, 
                               const string_array&  regexps);

    void addLibraryObjects(const Library&      library,
                           const std::string&  keyPrefix,
                           bool                ignoreDuplicates);

public:
    DatabaseManager();
	~DatabaseManager();
	
    LibraryCache&		getLibraryCache()		{ return libraryCache; }
	AnimationCache&		getAnimationCache()     { return animationCache; }
    EffectCache&        getEffectCache()        { return effectCache; }
    TextureCache&       getTextureCache()       { return textureCache; }
    VisualSceneCache&   getVisualSceneCache()   { return visualSceneCache; }
    LocationCache&      getLocationCache()      { return locationCache; }
#ifdef SLON_ENGINE_USE_PHYSICS
    PhysicsSceneCache&  getPhysicsSceneCache()  { return physicsSceneCache; }
#endif

    void clear(unsigned mask);
    library_ptr loadLibrary(const std::string& path, 
                            const std::string& keyPrefix, 
                            LibraryLoader&     loader,
                            bool               ignoreDuplicates = true);
    library_ptr loadLibrary(const std::string& path,
                            const std::string& keyPrefix,
                            format_id          format = library_format_auto,
                            bool               ignoreDuplicates = true);

    format_array getLibraryFormats() const										{ return libraryCache.getFormats(); }
    format_array getAppropriateLibraryFormats(const std::string& path) const	{ return libraryCache.getAppropriateFormats(path); }
    format_id    registerLibraryFormat(string_array pathExpr)                   { return libraryCache.registerFormat(pathExpr); }
    void         unregisterLibraryFormat(format_id format)						{ libraryCache.unregisterFormat(format); }
    void         clearLibraryFormats()											{ libraryCache.clearFormats(); }

    library_loader_array getLibraryLoaders() const													{ return libraryCache.getLoaders(); }
    library_loader_array getAppropriateLibraryLoaders(format_id format) const						{ return libraryCache.getAppropriateLoaders(format); }
    void                 registerLibraryLoader(format_id format, const library_loader_ptr& loader)	{ libraryCache.registerLoader(format, loader); }
    void                 unregisterLibraryLoader(format_id format, LibraryLoader* loader)			{ libraryCache.unregisterLoader(format, loader); }
    size_t               unregisterLibraryLoader(LibraryLoader* loader)								{ return libraryCache.unregisterLoader(loader); }
    void                 clearLibraryLoaders()														{ libraryCache.clearLoaders(); }
    
    library_saver_array  getLibrarySavers() const												{ return libraryCache.getSavers(); }
    library_saver_array  getAppropriateLibrarySavers(format_id format) const					{ return libraryCache.getAppropriateSavers(format); }
    void                 registerLibrarySaver(format_id format, const library_saver_ptr& saver) { libraryCache.registerSaver(format, saver); }
    void                 unregisterLibrarySaver(format_id format, LibrarySaver* saver)			{ libraryCache.unregisterSaver(format, saver); }
    size_t               unregisterLibrarySaver(LibrarySaver* saver)							{ return libraryCache.unregisterSaver(saver); }
    void                 clearLibrarySavers()													{ libraryCache.clearSavers(); }
	
	Serializable*		createSerializableByName(const std::string& name);
	bool				registerSerializableCreateFunc(const std::string& name, const serializable_create_func& func);
	bool				unregisterSerializableCreateFunc(const std::string& name);

private:
	format_desc* unwrap(format_id format) const;

private:
    LibraryCache        libraryCache;
	AnimationCache		animationCache;
    EffectCache         effectCache;
    TextureCache        textureCache;
    VisualSceneCache    visualSceneCache;
    LocationCache       locationCache;
#ifdef SLON_ENGINE_USE_PHYSICS
    PhysicsSceneCache   physicsSceneCache;
#endif

    format_desc_vector  formatDescs;
    format_loader_map   formatLoaders;
    format_saver_map    formatSavers;

	serializable_create_func_map serializableCreateFuncs;
};

template<typename T>
struct fmt_loader
{
    typedef boost::intrusive_ptr<T>             value_ptr;
    typedef database::Loader<value_ptr>         loader_type;
    typedef boost::intrusive_ptr<loader_type>   loader_ptr;

    const char*     fmtName;
    size_t          numExpr;
    const char*     fmtExpr[10];
    loader_type*    fmtLoader;
};

template<typename T>
inline void registerLoaders(size_t numLoaders, fmt_loader<T>* loaders)
{
    database::Cache<T>& cache = currentCache<T>();
    for (size_t i = 0; i<numLoaders; ++i)
    {
        std::vector<std::string> fmtExpr(loaders[i].fmtExpr, loaders[i].fmtExpr + loaders[i].numExpr);
        
        format_id fmtId = cache.registerFormat(fmtExpr);
        cache.registerLoader(fmtId, typename fmt_loader<T>::loader_ptr(loaders[i].fmtLoader));
    }
}

template<>
inline void registerLoaders<database::Library>(size_t numLoaders, fmt_loader<database::Library>* loaders)
{
    database::DatabaseManager& dm = currentDatabaseManager();
    for (size_t i = 0; i<numLoaders; ++i)
    {
        std::vector<std::string> fmtExpr(loaders[i].fmtExpr, loaders[i].fmtExpr + loaders[i].numExpr);
        
        format_id fmtId = dm.registerLibraryFormat(fmtExpr);
        dm.registerLibraryLoader(fmtId, library_loader_ptr(loaders[i].fmtLoader));
    }
}

template<typename T>
struct fmt_saver
{
    typedef boost::intrusive_ptr<T>             value_ptr;
    typedef database::Saver<value_ptr>          saver_type;
    typedef boost::intrusive_ptr<saver_type>    saver_ptr;

    const char*     fmtName;
    size_t          numExpr;
    const char*     fmtExpr[10];
    saver_type*     fmtSaver;
};

template<typename T>
inline void registerSavers(size_t numSavers, fmt_saver<T>* savers)
{
    database::Cache<T>& cache = currentCache<T>();
    for (size_t i = 0; i<numSavers; ++i)
    {
        std::vector<std::string> fmtExpr(savers[i].fmtExpr, savers[i].fmtExpr + savers[i].numExpr);
        
        format_id fmtId = cache.registerFormat(fmtExpr);
        cache.registerSaver(fmtId, typename fmt_saver<T>::saver_ptr(savers[i].fmtSaver));
    }
}

template<>
inline void registerSavers<database::Library>(size_t numSavers, fmt_saver<database::Library>* savers)
{
    database::DatabaseManager& manager = currentDatabaseManager();
    for (size_t i = 0; i<numSavers; ++i)
    {
        std::vector<std::string> fmtExpr(savers[i].fmtExpr, savers[i].fmtExpr + savers[i].numExpr);
        
        format_id fmtId = manager.registerLibraryFormat(fmtExpr);
        manager.registerLibrarySaver(fmtId, fmt_saver<database::Library>::saver_ptr(savers[i].fmtSaver));
    }
}

} // namespace detail
} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_DETAIL_DATABASE_MANAGER_H__
