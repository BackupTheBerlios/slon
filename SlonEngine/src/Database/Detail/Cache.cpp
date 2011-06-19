#include "stdafx.h"
#include "Animation/Animation.h"
#include "Database/Detail/Cache.h"
#include "Database/Library.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystemManager.h"
#include "Graphics/Effect.h"
#include "Scene/Node.h"
#include "Realm/Location.h"
#include "Realm/Object.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#include "Physics/PhysicsModel.h"
#endif
#include <boost/iostreams/stream_buffer.hpp>
#include <sgl/Texture.h>

namespace slon {
namespace database {
namespace detail {

template<typename T>
typename Cache<T>::format_desc* Cache<T>::unwrap(format_id format) const
{
    if (format.pObj)
    {
        format_desc* fdesc = reinterpret_cast<format_desc*>(format.pObj);
        if (fdesc->cache == this) {
            return fdesc;
        }
    }

    return 0;
}

template<typename T>
typename Cache<T>::value_ptr Cache<T>::loadImpl( const std::string& key,
                                                 const std::string& path, 
                                                 loader_type&       loader )
{
	using namespace filesystem;
	using namespace boost::iostreams;

    value_ptr value;
    try
    {
		// open file
		file_ptr file( asFile( currentFileSystemManager().getNode(path.c_str()) ) );
		if (!file) 
		{    
            (*logger) << log::S_ERROR << "Can't find file: " << path << LOG_FILE_AND_LINE;
			return value;
		}
		
        value = loader.load(file.get());
        if (value) {
            storage.insert( typename storage_type::value_type(key, value) );
        }
        else if (logger) {
            (*logger) << log::S_ERROR << "Unable to load item: " << path << LOG_FILE_AND_LINE;
        }
    }
    catch (loader_error& err) 
    {
        if (logger) {
            (*logger) << log::S_ERROR << err.what() << LOG_FILE_AND_LINE;
        }
    }

    return value;
}

template<typename T>
bool Cache<T>::saveImpl( const std::string& path, 
                         const value_ptr&   item,
                         saver_type&        saver )
{
	using namespace filesystem;
	using namespace boost::iostreams;

    try
    {
		// create file
		file_ptr file( currentFileSystemManager().createFile(path.c_str()) );
		if (!file) 
		{    
            (*logger) << log::S_ERROR << "Can't create file: " << path << LOG_FILE_AND_LINE;
			return false;
		}
		        
        saver.save(item, file.get());
    }
    catch (saver_error& err) 
    {
        if (logger) {
            (*logger) << log::S_ERROR << err.what() << LOG_FILE_AND_LINE;
        }
        
        return false;
    }

    return true;
}

template<typename T>
void Cache<T>::add(const std::string& key, const value_ptr& value)
{
    bool res = storage.insert( typename storage_type::value_type(key, value) ).second;
    assert(res);
}

template<typename T>
T* Cache<T>::find(const std::string& key)
{
    typename storage_type::iterator iter = storage.find(key);
    if ( iter != storage.end() ) {
        return iter->second.get();
    }

    return 0;
}

template<typename T>
typename Cache<T>::value_ptr Cache<T>::load( const std::string& key, 
                                             const std::string& path, 
                                             loader_type&       loader )
{
    value_ptr value( find(key) );
    if (value) {
        return value;
    }

    return loadImpl(key, path, loader);
}

template<typename T>
typename Cache<T>::value_ptr Cache<T>::load(const std::string& key,
                                            const std::string& path,
                                            format_id          format)
{
    value_ptr value( find(key) );
    if (value) {
        return value;
    }

    // choose formats
    format_array formats;
    if (format == base_type::format_auto) {
        formats = getAppropriateFormats(path);
    }
    else {
        formats.push_back(format);
    }

    // try loaders
    for (size_t i = 0; i<formats.size(); ++i)
    {
        format_desc* fdesc = unwrap(formats[i]);
        for (typename loader_set::iterator loaderIt  = fdesc->loaders.begin();
                                           loaderIt != fdesc->loaders.end();
                                           ++loaderIt)
        {
            if ( (value = loadImpl(key, path, **loaderIt)) ) {
                return value;
            }
        }
    }

    return value;
}

template<typename T>
bool Cache<T>::save(const std::string&  path,
			        const value_ptr&	item,
			        format_id			format)
{
    if (!item) {
        return false;
    }

    // choose formats
    format_array formats;
    if (format == base_type::format_auto) {
        formats = getAppropriateFormats(path);
    }
    else {
        formats.push_back(format);
    }

    // try savers
    for (size_t i = 0; i<formats.size(); ++i)
    {
        format_desc* fdesc = unwrap(formats[i]);
        for (typename saver_set::iterator saverIt  = fdesc->savers.begin();
                                          saverIt != fdesc->savers.end();
                                          ++saverIt)
        {
            if ( saveImpl(path, item, **saverIt) ) {
                return true;
            }
        }
    }

    return false;
}

template<typename T>
void Cache<T>::remove(const std::string& key)
{
    storage.erase(key);
}

template<typename T>
void Cache<T>::clear()
{
    storage.clear();
}

template<typename T>
typename Cache<T>::format_array Cache<T>::getFormats() const
{
    format_array formats;
    for (typename format_desc_list::iterator iter  = formatDescs.begin();
                                             iter != formatDescs.end();
                                             ++iter)
    {
        formats.push_back( format_id(&*iter) );
    }

    return formats;
}

template<typename T>
typename Cache<T>::format_array Cache<T>::getAppropriateFormats(const std::string& path) const
{
    using namespace boost::xpressive;

    std::vector<format_array> formats;
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        format_desc& fdesc = *fIt;
        for (size_t j = 0; j<fdesc.pathExprs.size(); ++j)
        {
            if ( regex_match(path, fdesc.pathExprs[j]) )
            {
                if (formats.size() < j + 1) {
                    formats.resize(j + 1);
                }
                formats[j].push_back( format_id(&fdesc) );
            }
        }
    }

    format_array formatsPlain;
    for (size_t i = 0; i<formats.size(); ++i)
    {
        for (size_t j = 0; j<formats[i].size(); ++j)
        {
            formatsPlain.push_back(formats[i][j]);
        }
    }

    return formatsPlain;
}

template<typename T>
format_id Cache<T>::registerFormat(string_array pathExpr)
{
    format_desc desc;
    desc.cache = this;
    desc.haveAnyMatch = false;
    
    boost::xpressive::sregex_compiler compiler;
    for (size_t i = 0; i<pathExpr.size(); ++i)
    {
        if (pathExpr[i] == "*") {
            desc.haveAnyMatch = true;
        }
        else {
            desc.pathExprs.push_back( compiler.compile(pathExpr[i]) );
        }
    }

    formatDescs.push_back(desc);
    return format_id( &formatDescs.back() );
}

template<typename T>
void Cache<T>::unregisterFormat(format_id format)
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        formatDescs.remove(*fdesc);    
    }
}

template<typename T>
void Cache<T>::clearFormats()
{
    formatDescs.clear();
}

template<typename T>
typename Cache<T>::loader_array Cache<T>::getLoaders() const
{
    loader_set loaderSet;
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        std::copy( fIt->loaders.begin(), fIt->loaders.end(), std::inserter(loaderSet, loaderSet.begin()) );
    }

    return loader_array(loaderSet.begin(), loaderSet.end() );
}

template<typename T>
typename Cache<T>::loader_array Cache<T>::getAppropriateLoaders(format_id format) const
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        return loader_array(fdesc->loaders.begin(), fdesc->loaders.end());
    }

    return loader_array();
}

template<typename T>
void Cache<T>::registerLoader(format_id format, const loader_ptr& loader)
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        fdesc->loaders.insert(loader);
    }
}

template<typename T>
void Cache<T>::unregisterLoader(format_id format, loader_type* loader)
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        fdesc->loaders.erase(loader_ptr(loader));
    }
}

template<typename T>
size_t Cache<T>::unregisterLoader(loader_type* loader)
{
    size_t count = 0;
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        count += fIt->loaders.erase(loader_ptr(loader));
    }

    return count;
}

template<typename T>
void Cache<T>::clearLoaders()
{
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        fIt->loaders.clear();
    }
}

template<typename T>
typename Cache<T>::saver_array Cache<T>::getSavers() const
{
    saver_set saverSet;
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        std::copy( fIt->savers.begin(), fIt->savers.end(), std::inserter(saverSet, saverSet.begin()) );
    }

    return saver_array(saverSet.begin(), saverSet.end());
}

template<typename T>
typename Cache<T>::saver_array Cache<T>::getAppropriateSavers(format_id format) const
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        return saver_array(fdesc->savers.begin(), fdesc->savers.end());
    }

    return saver_array();
}

template<typename T>
void Cache<T>::registerSaver(format_id format, const saver_ptr& saver)
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        fdesc->savers.insert(saver);
    }
}

template<typename T>
void Cache<T>::unregisterSaver(format_id format, saver_type* saver)
{
    format_desc* fdesc = unwrap(format);
    if (fdesc) {
        fdesc->savers.erase(saver_ptr(saver));
    }
}

template<typename T>
size_t Cache<T>::unregisterSaver(saver_type* saver)
{
    size_t count = 0;
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        count += fIt->savers.erase(saver_ptr(saver));
    }

    return count;
}

template<typename T>
void Cache<T>::clearSavers()
{
    for (typename format_desc_list::iterator fIt  = formatDescs.begin();
                                             fIt != formatDescs.end();
                                             ++fIt)
    {
        fIt->savers.clear();
    }
}

template class Cache<database::Library>;
template class Cache<animation::Animation>;
template class Cache<graphics::Effect>;
template class Cache<graphics::Texture>;
template class Cache<scene::Node>;
template class Cache<realm::Object>;
template class Cache<realm::Location>;
#ifdef SLON_ENGINE_USE_PHYSICS
template class Cache<physics::PhysicsModel>;
#endif

} // namespace detail
} // namespace database
} // namespace slon
