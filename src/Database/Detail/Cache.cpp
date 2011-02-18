#include "stdafx.h"
#include "Animation/Animation.h"
#include "Database/Detail/Cache.h"
#include "Database/Library.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystemManager.h"
#include "Graphics/Effect.h"
#include "Scene/Node.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#include "Physics/PhysicsModel.h"
#endif
#include <boost/iostreams/stream_buffer.hpp>
#include <sgl/Texture.h>

namespace slon {
namespace database {
namespace detail {

template<typename T>
typename Cache<T>::format_desc Cache<T>::makeFormatDesc(format_id id, const string_array& regexps)
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
		// determine file mode
		File::mask_t mode = File::in;
		if ( loader.binary() ) {
			mode |= File::binary;
		}

		// open file
		file_ptr file( asFile( currentFileSystemManager().getNode(path.c_str()) ) );
		if (!file) 
		{    
            (*logger) << log::S_ERROR << "Can't find file: " << path << LOG_FILE_AND_LINE;
			return value;
		}
		else if ( !file->open(mode)  )
		{
            (*logger) << log::S_ERROR << "Can't open file: " << path << LOG_FILE_AND_LINE;
			return value;
		}
		
		stream_buffer<filesystem::file_device> buf(file);
		std::istream stream(&buf);

        value = loader.load(stream);
        if (value) {
            storage.insert( storage_type::value_type(key, value) );
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
void Cache<T>::add(const std::string& key, const value_ptr& value)
{
    bool res = storage.insert( storage_type::value_type(key, value) ).second;
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

    if (format == base_type::format_auto)
    {
        format_array formats = getAppropriateFormats(path);
        for (size_t i = 0; i<formats.size(); ++i)
        {
            loader_array loaders = getAppropriateLoaders(formats[i]);
            for (size_t j = 0; j<loaders.size(); ++j)
            {
                if ( (value = loadImpl(key, path, *loaders[j])) ) {
                    return value;
                }
            }
        }
    }
    else 
    {
        loader_array loaders = getAppropriateLoaders(format);
        for (size_t j = 0; j<loaders.size(); ++j)
        {
            if ( (value = loadImpl(key, path, *loaders[j])) ) {
                return value;
            }
        }
    }

    return value;
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
    format_array formats( formatDescs.size() );
    std::transform( formatDescs.begin(), 
                    formatDescs.end(),
                    formats.begin(),
                    boost::bind(&format_desc::id, _1) );
    return formats;
}

template<typename T>
typename Cache<T>::format_array Cache<T>::getAppropriateFormats(const std::string& path) const
{
    using namespace boost::xpressive;

    std::vector<format_array> formats;
    for (size_t i = 0; i<formatDescs.size(); ++i)
    {
        for (size_t j = 0; j<formatDescs[i].pathExprs.size(); ++j)
        {
            if ( regex_match(path, formatDescs[i].pathExprs[j]) )
            {
                if (formats.size() < j + 1) {
                    formats.resize(j + 1);
                }
                formats[j].push_back(formatDescs[i].id);
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
void Cache<T>::registerFormat(format_id       format,
                              string_array    pathExpr)
{
    for (size_t i = 0; i<formatDescs.size(); ++i)
    {
        if (formatDescs[i].id == format) 
        {
            formatDescs[i] = makeFormatDesc(format, pathExpr);
            return;
        }
    }

    formatDescs.push_back( makeFormatDesc(format, pathExpr) );
}

template<typename T>
void Cache<T>::unregisterFormat(format_id format)
{
    quick_remove_if(formatDescs, boost::bind(&format_desc::id, _1) == format);
}

template<typename T>
void Cache<T>::clearFormats()
{
    formatDescs.clear();
}

template<typename T>
typename Cache<T>::loader_array Cache<T>::getLoaders() const
{
    loader_array loaders;
    for (typename format_loader_map::const_iterator iter  = formatLoaders.begin();
                                                    iter != formatLoaders.end();
                                                    ++iter)
    {
        std::copy( iter->second.begin(), iter->second.end(), std::back_inserter(loaders) );
    }

    return loaders;
}

template<typename T>
typename Cache<T>::loader_array Cache<T>::getAppropriateLoaders(format_id format) const
{
    typename format_loader_map::const_iterator iter = formatLoaders.find(format);
    if ( iter != formatLoaders.end() ) {
        return iter->second;
    }

    return loader_array();
}

template<typename T>
void Cache<T>::registerLoader(format_id format, const loader_ptr& loader)
{
    typename format_loader_map::iterator iter = formatLoaders.find(format);
    if ( iter != formatLoaders.end() )
    {
        typename loader_array::iterator it = std::find(iter->second.begin(), iter->second.end(), loader);
        if ( it == iter->second.end() ) {
            iter->second.push_back(loader);
        }
    }
    else 
    {
        loader_array loaders;
        loaders.push_back(loader);
        formatLoaders.insert( typename format_loader_map::value_type(format, loaders) );
    }
}

template<typename T>
void Cache<T>::unregisterLoader(format_id format, loader_type* loader)
{
    typename format_loader_map::iterator iter = formatLoaders.find(format);
    if ( iter != formatLoaders.end() ) {
        quick_remove( iter->second, loader_ptr(loader) );
    }
}

template<typename T>
size_t Cache<T>::unregisterLoader(loader_type* loader)
{
    size_t count = 0;
    for (typename format_loader_map::iterator iter  = formatLoaders.begin();
                                              iter != formatLoaders.end();
                                              ++iter)
    {
        count += quick_remove( iter->second, loader_ptr(loader) );
    }

    return count;
}

template<typename T>
void Cache<T>::clearLoaders()
{
    formatLoaders.clear();
}

template<typename T>
typename Cache<T>::saver_array Cache<T>::getSavers() const
{
    saver_array savers;
    for (typename format_saver_map::const_iterator iter  = formatSavers.begin();
                                                   iter != formatSavers.end();
                                                   ++iter)
    {
        std::copy( iter->second.begin(), iter->second.end(), std::back_inserter(savers) );
    }

    return savers;
}

template<typename T>
typename Cache<T>::saver_array Cache<T>::getAppropriateSavers(format_id format) const
{
    typename format_saver_map::const_iterator iter = formatSavers.find(format);
    if ( iter != formatSavers.end() ) {
        return iter->second;
    }

    return saver_array();
}

template<typename T>
void Cache<T>::registerSaver(format_id format, const saver_ptr& saver)
{
    typename format_saver_map::iterator iter = formatSavers.find(format);
    if ( iter != formatSavers.end() )
    {
        typename saver_array::iterator it = std::find(iter->second.begin(), iter->second.end(), saver);
        if ( it == iter->second.end() ) {
            iter->second.push_back(*it);
        }
    }
    else 
    {
        saver_array savers;
        savers.push_back(saver);
        formatSavers.insert( typename format_saver_map::value_type(format, savers) );
    }
}

template<typename T>
void Cache<T>::unregisterSaver(format_id format, saver_type* saver)
{
    typename format_saver_map::iterator iter = formatSavers.find(format);
    if ( iter != formatSavers.end() ) {
        quick_remove( iter->second, saver_ptr(saver) );
    }
}

template<typename T>
size_t Cache<T>::unregisterSaver(saver_type* saver)
{
    size_t count = 0;
    for (typename format_saver_map::iterator iter  = formatSavers.begin();
                                             iter != formatSavers.end();
                                             ++iter)
    {
        count += quick_remove( iter->second, saver_ptr(saver) );
    }

    return count;
}

template<typename T>
void Cache<T>::clearSavers()
{
    formatSavers.clear();
}

template class Cache<database::Library>;
template class Cache<animation::Animation>;
template class Cache<graphics::Effect>;
template class Cache<graphics::Texture>;
template class Cache<scene::Node>;
#ifdef SLON_ENGINE_USE_PHYSICS
template class Cache<physics::PhysicsModel>;
#endif

} // namespace detail
} // namespace database
} // namespace slon
