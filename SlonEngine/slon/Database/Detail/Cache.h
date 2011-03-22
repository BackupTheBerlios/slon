#ifndef __SLON_ENGINE_DATABASE_DETAIL_CACHE_H__
#define __SLON_ENGINE_DATABASE_DETAIL_CACHE_H__

#include "../../Utility/Algorithm/algorithm.hpp"
#include "../../Utility/Algorithm/prefix_tree.hpp"
#include "../Cache.h"
#include <boost/bind.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <map>
#include <sgl/Utility/Aligned.h>

namespace slon {
namespace database {
namespace detail {

/** Interface for accessing common objects in the storage. */
template<typename T>
class Cache :
    public database::Cache<T>
{
private:
    typedef T                                       value_type;
    typedef database::Cache<value_type>             base_type;

    typedef typename base_type::value_ptr           value_ptr;
    typedef typename base_type::loader_type         loader_type;
    typedef typename base_type::loader_ptr          loader_ptr;
    typedef typename base_type::saver_ptr           saver_ptr;

    typedef typename base_type::string_array        string_array;
    typedef typename base_type::format_array        format_array;
    typedef typename base_type::loader_array        loader_array;
    typedef typename base_type::saver_array         saver_array;

    typedef std::set<loader_ptr>                    loader_set;
    typedef std::set<saver_ptr>                     saver_set;

    typedef prefix_tree<char, value_ptr>            storage_type;

#if BOOST_VERSION < 104200 && __GNUC__
    typedef sgl::vector<boost::xpressive::sregex>   regex_vector;
#else
    typedef std::vector<boost::xpressive::sregex>   regex_vector;
#endif

    struct format_desc
    {
        Cache*       cache;
        regex_vector pathExprs;
        bool         haveAnyMatch;
        loader_set   loaders;
        saver_set    savers;

        bool operator == (const format_desc& rhs) const { return this == &rhs; }
    };

    typedef std::list<format_desc>                  format_desc_list;

private:
    format_desc* unwrap(format_id format) const;
    
    value_ptr loadImpl(const std::string& key,
                       const std::string& path, 
                       loader_type&       loader);

    bool saveImpl( const std::string& path,
                   const value_ptr&   item,
                   saver_type&        saver );

public:
    Cache(log::Logger* logger_ = 0) 
    :   logger(logger_) 
    {}

    // Override Cache
    void      add(const std::string& key, const value_ptr& value);
    T*        find(const std::string& key);
    value_ptr load(const std::string& key, 
                   const std::string& path, 
                   loader_type&       loader);
    value_ptr load(const std::string& path, 
                   loader_type&       loader) { return load(path, path, loader); }
    value_ptr load(const std::string& key,
                   const std::string& path,
                   format_id          format = base_type::format_auto);
    value_ptr load(const std::string& path,
                   format_id          format = base_type::format_auto) { return load(path, path, format); }
    bool      save(const std::string& path,
				   const value_ptr&	  item,
				   format_id		  format = base_type::format_auto);

    void      remove(const std::string& key);
    void      clear();

    format_array getFormats() const;
    format_array getAppropriateFormats(const std::string& path) const;
    format_id    registerFormat(string_array pathExpr);
    void         unregisterFormat(format_id format);
    void         clearFormats();

    loader_array getLoaders() const;
    loader_array getAppropriateLoaders(format_id format) const;
    void         registerLoader(format_id format, const loader_ptr& loader);
    void         unregisterLoader(format_id format, loader_type* loader);
    size_t       unregisterLoader(loader_type* loader);
    void         clearLoaders();

	saver_array  getSavers() const;
    saver_array  getAppropriateSavers(format_id format) const;
	void         registerSaver(format_id format, const saver_ptr& saver);
	void         unregisterSaver(format_id format, saver_type* saver);
	size_t       unregisterSaver(saver_type* saver);
	void         clearSavers();

private:
    log::Logger*                logger;
    storage_type                storage;
    mutable format_desc_list    formatDescs;
};

} // namespace detail
} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_DETAIL_CACHE_H__
