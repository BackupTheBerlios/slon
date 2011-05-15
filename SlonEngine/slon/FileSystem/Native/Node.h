#ifndef __FILESYSTEM_NATIVE_NODE_H__
#define __FILESYSTEM_NATIVE_NODE_H__

#include <boost/filesystem.hpp>
#include "../../Utility/Algorithm/prefix_tree.hpp"
#include "../Detail/FileSystemManager.h"
#include "../Directory.h"
#include "../File.h"

namespace slon {
namespace filesystem {
namespace native {

// Forward
class Directory;
	
template<typename Base>
class Node :
	public Base
{
public:
    Node(detail::FileSystemManager*         manager,
         const boost::filesystem::path&     systemPath,
         const boost::filesystem::path&     virtualPath);
	~Node();

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

} // namespace native
} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_NATIVE_NODE_H__
