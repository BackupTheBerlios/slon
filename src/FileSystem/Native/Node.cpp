#include "stdafx.h"
#include "FileSystem/Detail/FileSystemManager.h"
#include "FileSystem/Native/Directory.h"

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace native {

template<typename Base>
Node<Base>::Node(detail::FileSystemManager*         manager_,
                 const boost::filesystem::path&     systemPath_,
                 const boost::filesystem::path&     virtualPath_)
:	manager(manager_)
{
    virtualPath = virtualPath_;
    systemPath  = boost::fs::system_complete(systemPath_);
    name        = virtualPath.filename();
    vpath       = virtualPath.string();
    manager->registerNode(vpath, this);
}

template<typename Base>
Node<Base>::~Node()
{
    manager->unregisterNode(vpath);
}

template class Node<filesystem::Directory>;
template class Node<filesystem::File>;

} // namespace native
} // namespace filesystem
} // namespace slon
