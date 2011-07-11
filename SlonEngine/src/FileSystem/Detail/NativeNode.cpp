#include "stdafx.h"
#include "FileSystem/Detail/FileSystemManager.h"
#include "FileSystem/Detail/NativeDirectory.h"

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace detail {

template<typename Base>
NativeNode<Base>::NativeNode(detail::FileSystemManager*         manager_,
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
NativeNode<Base>::~NativeNode()
{
    manager->unregisterNode(vpath);
}

template class NativeNode<filesystem::Directory>;
template class NativeNode<filesystem::File>;

} // namespace detail
} // namespace filesystem
} // namespace slon
