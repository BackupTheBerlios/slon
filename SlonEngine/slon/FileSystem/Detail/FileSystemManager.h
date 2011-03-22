#ifndef __FILESYSTEM_DETAIL_FILESYSTEM_MANAGER_H__
#define __FILESYSTEM_DETAIL_FILESYSTEM_MANAGER_H__

#include "../../Utility/Algorithm/prefix_tree.hpp"
#include "../Directory.h"
#include "../FileSystemManager.h"
#include <boost/filesystem.hpp>
#include <list>

namespace slon {
namespace filesystem {
namespace detail {

class FileSystemManager :
	public filesystem::FileSystemManager
{
public:
    typedef std::list<boost::filesystem::path>      path_list;
    typedef prefix_tree<char, filesystem::Node*>    node_prefix_tree;
    typedef prefix_tree<char, path_list>            path_list_prefix_tree;

public:
	// Override FileSystemManager
	filesystem::Directory*	setWorkingDirectory(const char* virtualPath);
    bool            		mount(const char* systemPath, const char* mountPoint);
    bool                    unmount(const char* systemPath, const char* virtualPath);
	filesystem::Node*		getNode(const char* path);
    bool                    removeNode(Node* node);

    filesystem::Directory*  createDirectory(const char* name);
    filesystem::File*       createFile(const char* path);

public:
    void registerNode(const std::string& sysPath, filesystem::Node* node);
    void unregisterNode(const std::string& sysPath);

private:
    node_ptr                workingDir;
    path_list_prefix_tree	mountPoints;
    node_prefix_tree        activeNodes;
};

typedef boost::intrusive_ptr<FileSystemManager> file_system_manager_ptr;

} // namespace detail
} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_DETAIL_FILESYSTEM_MANAGER_H__
