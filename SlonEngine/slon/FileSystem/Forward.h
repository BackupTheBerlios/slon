#ifndef __SLON_ENGINE_FILESYSTEM_FORWARD_H__
#define __SLON_ENGINE_FILESYSTEM_FORWARD_H__

namespace boost
{
    template<typename T>
    class intrusive_ptr;
}

namespace slon {
namespace filesystem {

    class Directory;
    class File;
    class Node;

    typedef boost::intrusive_ptr<Directory>     directory_ptr;
    typedef boost::intrusive_ptr<File>          file_ptr;
    typedef boost::intrusive_ptr<Node>          node_ptr;

} // namespace filesystem
} // namespace slon

#endif // __SLON_ENGINE_FILESYSTEM_FORWARD_H__