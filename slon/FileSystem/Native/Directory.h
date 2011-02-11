#ifndef __FILESYSTEM_NATIVE_DIRECTORY_H__
#define __FILESYSTEM_NATIVE_DIRECTORY_H__

#include "Node.h"

namespace slon {
namespace filesystem {
namespace native {

class Directory :
	public Node<filesystem::Directory>
{
private:
	typedef std::vector<boost::filesystem::path>	path_vector;
	typedef std::vector<node_ptr>					node_vector;

public:
    Directory(detail::FileSystemManager*        manager,
              const boost::filesystem::path&    systemPath,
              const boost::filesystem::path&    virtualPath,
              bool                              initialize = false);

    // Override Node
	void reload();
	void flush();

	// Override Directory
    unsigned				getNumChildren() const;
    filesystem::Node*		getChild(unsigned id);
    const filesystem::Node* getChild(unsigned id) const;
    filesystem::Node*		getChild(const char* name);
    const filesystem::Node* getChild(const char* name) const;

	/** Add virtual child node. */
	void addVirtualNode(filesystem::Node* node);

	/** Remove child node from search path. */
	bool removeVirtualNode(filesystem::Node* node);

private:
	path_vector	mountPoints;
	node_vector	nodes;
};

} // namespace native
} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_NATIVE_DIRECTORY_H__