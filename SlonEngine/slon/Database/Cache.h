#ifndef __SLON_ENGINE_DATABASE_CACHE_H__
#define __SLON_ENGINE_DATABASE_CACHE_H__

#include <vector>
#include "../Utility/handle.hpp"
#include "Loader.h"
#include "Saver.h"

namespace slon {
namespace database {

/** File format handle. */
typedef handle format_id;

/** Interface for accessing common objects in the storage. */
template<typename T>
class SLON_PUBLIC Cache :
    public Referenced
{
public:
    typedef T                                   value_type;
    typedef boost::intrusive_ptr<value_type>    value_ptr;
    typedef Loader<value_ptr>                   loader_type;
    typedef boost::intrusive_ptr<loader_type>   loader_ptr;
    typedef Saver<value_ptr>                    saver_type;
    typedef boost::intrusive_ptr<saver_type>    saver_ptr;

    typedef std::vector<std::string>    string_array;
    typedef std::vector<format_id>      format_array;
    typedef std::vector<loader_ptr>     loader_array;
    typedef std::vector<saver_ptr>      saver_array;

    static const format_id              format_auto;

public:
    /** Look for item in the cache.
     * @param key - key for searching item in the storage.
     * @param return item if it has been found in the storage. Otherwise return NULL.
     */
    virtual T* find(const std::string& key) = 0;

    /** Look for item in the cache. If not found then call loader and add item into storage.
     * @param key - key for searching item in the storage.
     * @param path - argument for loader to load item.
     * @param loader - item loader.
     * @param return item if it has been succesfully loaded. Otherwise return NULL.
     */
    virtual value_ptr load(const std::string& key, 
                           const std::string& path, 
                           loader_type&       loader) = 0;

    /** Look for item in the cache. If not found then call loader and add item into storage.
     * @param path - key for searching item in the storage and argument for loader to load item.
     * @param loader - item loader.
     * @param return item if it has been succesfully loaded. Otherwise return NULL.
     */
    virtual value_ptr load(const std::string& path, 
                           loader_type&       loader) = 0;

    /** Look for item in the stroage, if not found try to load item from file and add into the storage.
     * Function will try every loader specified for item format.
     * @param key - key for searching item in the storage.
     * @param path - file path.
     * @param format - item file format. If format == format_auto, function will try any format 
     * returned by getAppropriateFormats(path) function.
     * @param return item if it has been succesfully loaded. Otherwise return NULL.
     * @see getAppropriateFormats, registerFormat, getAppropriateLoaders, registerLoader 
     */
    virtual value_ptr load(const std::string& key,
                           const std::string& path,
                           format_id          format = format_auto) = 0;

    /** Look for item in the stroage, if not found try to load item from file and add into the storage.
     * Function will try every loader specified for item format.
     * @param path - key for searching item in the storage and file path.
     * @param format - item file format. If format == format_auto, function will try any format 
     * returned by getAppropriateFormats(path) function.
     * @param return item if it has been succesfully loaded. Otherwise return NULL.
     * @see getAppropriateFormats, registerFormat, getAppropriateLoaders, registerLoader 
     */
    virtual value_ptr load(const std::string& path,
                           format_id          format = format_auto) = 0;

    /** Remove item with specified key from the cache */
    virtual void remove(const std::string& key) = 0;

    /** Remove objects from the cache. */
    virtual void clear() = 0;

    /** Get all registered formats. */
    virtual format_array getFormats() const = 0;

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
    virtual format_array getAppropriateFormats(const std::string& path) const = 0;

    /** Register item format. If format already present - replace it.
     * @param format - format identifier.
     * @param pathExpr - regular expressions for determining format using file name (e.g ".*\\.(?i:jpg)", ".*\\.(?i:jpeg)", ".*"). 
     * List any expression - ".*" if format can be determined by the file source. Expressions are case sensitive.
     * Order of expressions in the array will be used to determine probability of matching format. Any expression ".*"
     * has the lowest priority.
     * @see getAppropriateFormats
     */
    virtual format_id registerFormat(string_array pathExpr) = 0;

    /** Unregister item format. Function will not remove format loaders. */
    virtual void unregisterFormat(format_id format) = 0;

    /** Remove all formats, savers and loaders from the cache. */
    virtual void clearFormats() = 0;

    /** Get all registered loaders. */
    virtual loader_array getLoaders() const = 0;

    /** Get loaders for the specified format. Order of loaders in the array is not specified. */
    virtual loader_array getAppropriateLoaders(format_id format) const = 0;

    /** Register loader for specified format, replace existent if present.
     * @param format - format for which loader is specified.
     * @param loader - new loader for provided format.
     */
    virtual void registerLoader(format_id format, const loader_ptr& loader) = 0;

    /** Unregister loader for specified format. 
     * @param format - format supported by loader.
     * @param loader - loader for removal.
     */
    virtual void unregisterLoader(format_id format, loader_type* loader) = 0;

    /** Unregister loader for all formats.
     * @param loader - loader for removal.
     * @return number of formats for which loader was unregistered.
     */
    virtual size_t unregisterLoader(loader_type* loader) = 0;
	
    /** Remove all loaders from the cache */
    virtual void clearLoaders() = 0;
	
    /** Save physics item to the file.
     * @param path - path to the file.
     * @param scene - item to save.
     * @param format - format of the physics scene to save.
     * @return true on success.
     */
    virtual bool save(const std::string& path,
					  const value_ptr&	 item,
					  format_id          format = format_auto) = 0;

    /** Get all registered saver. */
    virtual saver_array getSavers() const = 0;
	
    /** Get savers for the specified format. Order of savers in the array is not specified. */
    virtual saver_array getAppropriateSavers(format_id format) const = 0;

    /** Register saver for specified format, replace existent if present.
     * @param saver - format for which saver is specified.
     * @param saver - new saver for provided format.
     */
	virtual void registerSaver(format_id format, const saver_ptr& saver) = 0;
	
    /** Unregister saver for specified format. 
     * @param format - format supported by loader.
     * @param saver - saver for removal.
     */
	virtual void unregisterSaver(format_id format, saver_type* saver) = 0;

    /** Unregister saver for all formats.
     * @param saver - saver for removal.
     * @return number of formats for which saver was unregistered.
     */
	virtual size_t unregisterSaver(saver_type* saver) = 0;

    /** Remove all savers from the cache */
    virtual void clearSavers() = 0;

    virtual ~Cache() {}
};

template<typename T>
const format_id Cache<T>::format_auto = format_id(0);

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_CACHE_H__
