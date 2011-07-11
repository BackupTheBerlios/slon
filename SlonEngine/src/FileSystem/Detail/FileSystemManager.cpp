#include "stdafx.h"
#include "Engine.h"
#include "FileSystem/Detail/FileSystemManager.h"
#include "FileSystem/Detail/NativeDirectory.h"
#include "FileSystem/Detail/NativeFile.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"
#include <cstring>
#include <stack>

DECLARE_AUTO_LOGGER("filesystem.FileSystemManager")

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace detail {
	
FileSystemManager::~FileSystemManager()
{
}

filesystem::Directory* FileSystemManager::setWorkingDirectory(const char* virtualPath)
{
	filesystem::Directory* dir = asDirectory( getNode(virtualPath) );
	if (dir) {
		workingDir.reset(dir);
	}
	return dir;
}

bool FileSystemManager::mount(const char* systemPath, const char* mountPoint)
{
#ifdef SLON_ENGINE_USE_PHYS_FS
#else
	boost::fs::path path(systemPath);
	if ( boost::fs::exists(path) )
    {
        std::string mountPointStr(mountPoint);
        path_list_prefix_tree::iterator pathListIter = mountPoints.find(mountPointStr);
        if ( pathListIter == mountPoints.end() ) {
            pathListIter = mountPoints.insert( path_list_prefix_tree::value_type(mountPointStr, path_list()) ).first;
        }
        pathListIter->second.push_back(path);

        if ( mountPointStr == "/" )
        {
            workingDir.reset();
            if ( boost::fs::is_regular_file(path) ) {
                workingDir.reset( new NativeFile(this, path, boost::fs::path(mountPointStr)) );
            }
            else if ( boost::fs::is_directory(path) ) {
                workingDir.reset( new NativeDirectory(this, path, boost::fs::path(mountPointStr), true) );
            }
        }

        return true;
	}

    return false;
#endif
}

bool FileSystemManager::unmount(const char* systemPath, const char* virtualPath)
{
    return false;
}

filesystem::Node* FileSystemManager::getNode(const char* path)
{
    std::string pathStr(path);
    if (pathStr.length() >= 2 && pathStr[0] == '.' && pathStr[1] == '/') {
        pathStr = workingDir->getPath() + pathStr.substr(2);
    }
    else if (pathStr[0] != '/') {
        pathStr = workingDir->getPath() + pathStr;
    }

    path_list_prefix_tree::iterator pathListIter = mountPoints.find_nearest(pathStr);
    if ( pathListIter != mountPoints.end() )
    {
        for (path_list::iterator iter  = pathListIter->second.begin();
                                 iter != pathListIter->second.end();
                                 ++iter)
        {
            boost::fs::path systemPath(*iter);
            systemPath /= pathStr;

            // try to find active node
            if ( boost::fs::exists(systemPath) )
            {
                std::string systemPathStr = systemPath.string();
                node_prefix_tree::iterator activeIter = activeNodes.find(systemPathStr);
                if ( activeIter != activeNodes.end() ) {
                    return activeIter->second;
                }

                // create node
                if ( boost::fs::is_regular_file(systemPath) ) {
                    return new NativeFile(this, systemPath, boost::fs::path(path));
                }
                else if ( boost::fs::is_directory(systemPath) ) {
                    return new NativeDirectory(this, systemPath, boost::fs::path(path), true);
                }
            }
        }
    }

    return 0;
}

bool FileSystemManager::removeNode(Node* node)
{
    return false;
}

filesystem::Directory* FileSystemManager::createDirectory(const char* name)
{
    return 0;
}

filesystem::File* FileSystemManager::createFile(const char* path)
{
    if ( filesystem::File* file = asFile( getNode(path) ) ) {
        return file;
    }

    boost::fs::path  vPath(path);
    NativeDirectory* parentDir = static_cast<NativeDirectory*>( asDirectory( getNode(vPath.parent_path().string().c_str()) ) );
    if (!parentDir) {
        return 0;
    }

    boost::fs::path sPath = parentDir->getSystemPath() / vPath.filename();
    std::ofstream file( sPath.string().c_str() );

    return new NativeFile(this, sPath, vPath);
}

void FileSystemManager::registerNode(const std::string& sysPath, filesystem::Node* node)
{
    std::pair<node_prefix_tree::iterator, bool> insPair = activeNodes.insert( node_prefix_tree::value_type(sysPath, node) );
    assert(insPair.second);
}

void FileSystemManager::unregisterNode(const std::string& sysPath)
{
    node_prefix_tree::iterator iter = activeNodes.find(sysPath);
    assert( iter != activeNodes.end() );
    activeNodes.erase(iter);
}

} // namespace detail

FileSystemManager& currentFileSystemManager()
{
	return Engine::Instance()->getFileSystemManager();
}

} // namespace filesystem
} // namespace slon
