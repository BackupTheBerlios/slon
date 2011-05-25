#ifndef __FILESYSTEM_FILE_H__
#define __FILESYSTEM_FILE_H__

#include <boost/iostreams/categories.hpp>
#include "Node.h"

namespace slon {
namespace filesystem {

/** Interface for reading/writing from/to virtual files. 
 * File is also compatible with boost::iostreams device to make it easier
 * to define streams based on files.
 */
class File :
	public Node
{
public:
    typedef char									char_type;
    typedef boost::iostreams::seekable_device_tag	category;

	enum
	{
		append   = 1 << 0,
		at_end   = 1 << 1,
		binary	 = 1 << 2,
		in       = 1 << 3,
		out      = 1 << 4,
		truncate = 1 << 5
	};
	
	typedef int mask_t;

public:
	virtual bool open(mask_t mode) = 0;

	virtual void close() = 0;

	virtual bool isOpen() const = 0;

	virtual bool eof() const = 0;

	virtual std::streampos tell() const = 0; 

	virtual std::streamsize size() const = 0;

	virtual std::streampos seek(std::streamoff off, std::ios_base::seekdir way) = 0;

	virtual std::streamsize read(char* buffer, std::streamsize size) = 0;

	virtual std::streamsize write(const char* buffer, std::streamsize size) = 0;

	virtual ~File() {}
};

typedef boost::intrusive_ptr<File>       file_ptr;
typedef boost::intrusive_ptr<const File> const_file_ptr;

/** boost::iostreams compatible file device.
 * Example of constructing istream from filesystem::File:
 * \code
 * filesystem::file_ptr file( filesystem::asFile( filesystem::currentFileSystemManager().getNode("Data/image.png") ) );
 * boost::iostreams::stream_buffer<filesystem::file_device> buf(*file);
 * std::istream is(&buf);
 * std::ostream os(&buf);
 * \uncode
 */
class file_device
{
public:
	typedef File::char_type	char_type;
    typedef File::category	category;

public:
	/** Aware! Implicit constructor */
	file_device(File& file_)
	:	file(file_)
	{}

	// Implement device
	std::streampos seek(std::streamoff off, std::ios_base::seekdir way) 
	{ 
		return file.seek(off, way);
	}

	std::streamsize read(char* buffer, std::streamsize size)
	{
		return file.read(buffer, size);
	}

	std::streamsize write(const char* buffer, std::streamsize size)
	{
		return file.write(buffer, size);
	}

private:
	File& file;
};

/** Try interpret node as file */
inline File* asFile(Node* node) 
{
	return (node && node->getType() == Node::FILE) ? static_cast<File*>(node) : 0;
}

/** Try interpret node as file */
inline const File* asFile(const Node* node) 
{
	return (node && node->getType() == Node::FILE) ? static_cast<const File*>(node) : 0;
}

} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_FILE_H__
