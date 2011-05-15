#include "stdafx.h"
#include "FileSystem/Native/Directory.h"
#include "FileSystem/Native/File.h"
#include "Utility/error.hpp"

__DEFINE_LOGGER__("filesystem.FileSystemManager")

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace native {

Directory::Directory(detail::FileSystemManager*     manager,
                     const boost::filesystem::path& systemPath,
                     const boost::filesystem::path& virtualPath,
                     bool                           initialize)
:   native::Node<filesystem::Directory>(manager, systemPath, virtualPath)
{
    if ( !boost::fs::is_directory(systemPath) ) {
        throw slon_error(logger, "Directory::Directory failed. Can't initialized directory from non-directory path");
    }

    if (initialize) {
        reload();
    }
}

// Override Node
void Directory::reload()
{
	boost::fs::directory_iterator end_iter;
    for ( boost::fs::directory_iterator iter(systemPath);
										iter != end_iter;
										++iter )
	{
		if ( boost::fs::is_directory( iter->status() ) ) 
		{
            Directory* directory = new Directory( manager, iter->path(), virtualPath / iter->filename() );
			nodes.push_back(directory);
		}
		else if ( boost::fs::is_regular_file( iter->status() ) ) 
		{
            File* file = new File( manager, iter->path(), virtualPath / iter->filename() );
			nodes.push_back(file);
		}
	}
}

void Directory::flush()
{
	// when using native fs changes are immediately flushed
}

// Override Directory
unsigned Directory::getNumChildren() const
{
	return nodes.size();
}

filesystem::Node* Directory::getChild(unsigned id)
{
	return nodes[id].get();
}

const filesystem::Node* Directory::getChild(unsigned id) const
{
	return nodes[id].get();
}

filesystem::Node* Directory::getChild(const char* name)
{
	node_vector::iterator iter = std::find_if( nodes.begin(),
											   nodes.end(),
											   boost::bind(strcmp, boost::bind(&filesystem::Node::getName, _1), name) == 0 );
	if ( iter != nodes.end() ) {
		return (*iter).get();
	}

	return 0;
}

const filesystem::Node* Directory::getChild(const char* name) const
{
	node_vector::const_iterator iter = std::find_if( nodes.begin(),
													 nodes.end(),
													 boost::bind(strcmp, boost::bind(&filesystem::Node::getName, _1), name) == 0 );
	if ( iter != nodes.end() ) {
		return (*iter).get();
	}

	return 0;
}

} // namespace native
} // namespace filesystem
} // namespace slon
