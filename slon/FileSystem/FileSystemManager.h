#ifndef __FILESYSTEM_FILESYSTEM_MANAGER_H__
#define __FILESYSTEM_FILESYSTEM_MANAGER_H__

#include "../Utility/referenced.hpp"

namespace slon {
namespace filesystem {

// forward
class Node;
class Directory;
class File;

class FileSystemManager :
    public Referenced
{
public:
	/** Set current working directory(path).
	 * @return node of the working directory or 0 if directory not found or it is not directory.
	 */
	virtual Directory* setWorkingDirectory(const char* virtualPath) = 0;

	/** Mount node to the engine file system.
	 * @systemPath - system path to the directory or archive.
	 * @mountPoint - mount point directory.
     * @return true if succeeded.
	 */
    virtual bool mount(const char* systemPath, const char* virtualPath) = 0;

    /** Unmount path from file system.
     * @return true if succeeded.
     */
    virtual bool unmount(const char* systemPath, const char* virtualPath) = 0;
	
	/** Get node of the engine file system with specified path. */
	virtual Node* getNode(const char* path) = 0;

    /** Remove node from native and engine file system.
     * @return true if succeeded.
     */
    virtual bool removeNode(Node* node) = 0;

    /** Create directory with specified path */
    virtual Directory* createDirectory(const char* name) = 0;

    /** Create file with dpecified path */
    virtual File* createFile(const char* path) = 0;

	virtual ~FileSystemManager() {}
};

/** Get engine file system manager */
FileSystemManager& currentFileSystemManager();

} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_FILESYSTEM_MANAGER_H__
