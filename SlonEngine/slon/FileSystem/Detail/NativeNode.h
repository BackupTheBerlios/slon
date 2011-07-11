#ifndef __FILESYSTEM_DETAIL_NATIVE_NODE_H__
#define __FILESYSTEM_DETAIL_NATIVE_NODE_H__

#include <boost/filesystem.hpp>
#include "../../Utility/Algorithm/prefix_tree.hpp"
#include "../Directory.h"
#include "../File.h"
#include "FileSystemManager.h"

namespace slon {
namespace filesystem {
namespace detail {

// Forward
class Directory;
	
template<typename Base>
class NativeNode :
	public Base
{
public:
    NativeNode(detail::FileSystemManager*         manager,
               const boost::filesystem::path&     systemPath,
               const boost::filesystem::path&     virtualPath);
    ~NativeNode();

    /** Setup system path for node */
    virtual void reset(const boost::filesystem::path& systemPath_) { systemPath = systemPath_; }

    /** Get native node path */
    virtual const boost::filesystem::path& getSystemPath() const { return systemPath; }

    // Override Node
    const char*	getPath() const { return vpath.c_str(); }
    const char*	getName() const { return name.c_str(); }

protected:
    detail::FileSystemManager*  manager;
    boost::filesystem::path     virtualPath;	// vfs path
    boost::filesystem::path     systemPath;     // system path
    std::string                 vpath;
    std::string                 name;
};

} // namespace detail
} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_DETAIL_NATIVE_NODE_H__
