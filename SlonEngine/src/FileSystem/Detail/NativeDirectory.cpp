#include "stdafx.h"
#include "FileSystem/Detail/NativeDirectory.h"
#include "FileSystem/Detail/NativeFile.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("filesystem.FileSystemManager")

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace detail {

NativeDirectory::NativeDirectory(detail::FileSystemManager*     manager,
                                 const boost::filesystem::path& systemPath,
                                 const boost::filesystem::path& virtualPath,
                                 bool                           initialize)
:   NativeNode<filesystem::Directory>(manager, systemPath, virtualPath)
{
    if ( !boost::fs::is_directory(systemPath) ) {
        throw slon_error(AUTO_LOGGER, "Directory::Directory failed. Can't initialized directory from non-directory path");
    }

    if (initialize) {
        reload();
    }
}

// Override Node
void NativeDirectory::reload()
{
	boost::fs::directory_iterator end_iter;
    for ( boost::fs::directory_iterator iter(systemPath);
										iter != end_iter;
										++iter )
	{
		if ( boost::fs::is_directory( iter->status() ) ) 
		{
            NativeDirectory* directory = new NativeDirectory( manager, iter->path(), virtualPath / iter->filename() );
			nodes.push_back(directory);
		}
		else if ( boost::fs::is_regular_file( iter->status() ) ) 
		{
            NativeFile* file = new NativeFile( manager, iter->path(), virtualPath / iter->filename() );
			nodes.push_back(file);
		}
	}
}

void NativeDirectory::flush()
{
	// when using native fs changes are immediately flushed
}

// Override Directory
unsigned NativeDirectory::getNumChildren() const
{
	return nodes.size();
}

filesystem::Node* NativeDirectory::getChild(unsigned id)
{
	return nodes[id].get();
}

const filesystem::Node* NativeDirectory::getChild(unsigned id) const
{
	return nodes[id].get();
}

filesystem::Node* NativeDirectory::getChild(const char* name)
{
	node_vector::iterator iter = std::find_if( nodes.begin(),
											   nodes.end(),
											   boost::bind(strcmp, boost::bind(&filesystem::Node::getName, _1), name) == 0 );
	if ( iter != nodes.end() ) {
		return (*iter).get();
	}

	return 0;
}

const filesystem::Node* NativeDirectory::getChild(const char* name) const
{
	node_vector::const_iterator iter = std::find_if( nodes.begin(),
													 nodes.end(),
													 boost::bind(strcmp, boost::bind(&filesystem::Node::getName, _1), name) == 0 );
	if ( iter != nodes.end() ) {
		return (*iter).get();
	}

	return 0;
}

} // namespace detail
} // namespace filesystem
} // namespace slon
