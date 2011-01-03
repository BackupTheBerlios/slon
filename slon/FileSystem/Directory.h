#ifndef __FILESYSTEM_DIRECTORY_H__
#define __FILESYSTEM_DIRECTORY_H__

#include "Node.h"

namespace slon {
namespace filesystem {

// forward
class File;

class Directory :
	public Node
{
public:
    virtual unsigned getNumChildren() const = 0;

    virtual Node* getChild(unsigned id) = 0;

    virtual const Node* getChild(unsigned id) const = 0;

    virtual Node* getChild(const char* name) = 0;

    virtual const Node* getChild(const char* name) const = 0;

    /** Reload directory content from disc. */
    virtual void reload() = 0;

	virtual ~Directory() {}
};

typedef boost::intrusive_ptr<Directory>			directory_ptr;
typedef boost::intrusive_ptr<const Directory>	const_directory_ptr;

/** Try interpret node as directory */
inline Directory* asDirectory(Node* node) 
{
	return (node && node->getType() == Node::DIRECTORY) ? static_cast<Directory*>(node) : 0;
}

/** Try interpret node as directory */
inline const Directory* asDirectory(const Node* node) 
{
	return (node && node->getType() == Node::DIRECTORY) ? static_cast<const Directory*>(node) : 0;
}

} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_DIRECTORY_H__
