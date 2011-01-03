#ifndef __FILESYSTEM_NODE_H__
#define __FILESYSTEM_NODE_H__

#include "../Utility/referenced.hpp"
#include <boost/intrusive_ptr.hpp>

namespace slon {
namespace filesystem {

class Node :
	public Referenced
{
public:
	enum TYPE
	{
		FILE,
		DIRECTORY
	};

public:
	virtual TYPE getType() const = 0;

	virtual const char* getPath() const = 0;

	virtual const char* getName() const = 0;

	/** Dump node content to the system */
	virtual void flush() = 0;

	virtual ~Node() {}
};

typedef boost::intrusive_ptr<Node>       node_ptr;
typedef boost::intrusive_ptr<const Node> const_node_ptr;

} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_NODE_H__