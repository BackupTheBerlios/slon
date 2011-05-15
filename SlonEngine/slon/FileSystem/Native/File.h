#ifndef __FILESYSTEM_NATIVE_FILE_H__
#define __FILESYSTEM_NATIVE_FILE_H__

#include <cstdio>
#include "Node.h"

namespace slon {
namespace filesystem {
namespace native {

class File :
	public Node<filesystem::File>
{
public:
    File(detail::FileSystemManager*         manager,
         const boost::filesystem::path&     systemPath,
         const boost::filesystem::path&     virtualPath);

	// Override Node
	filesystem::Node::TYPE  getType() const { return filesystem::Node::FILE; }
	void                    reload();
	void                    flush();

	// Override File
	bool	open(mask_t mode);
	void	close();
	bool	isOpen() const;

	bool			eof() const;
	std::streampos	tell() const; 
	std::streamsize	size() const;
	std::streampos  seek(std::streamoff off, std::ios_base::seekdir way);

	std::streamsize	read(char* buffer, std::streamsize size);
	std::streamsize	write(const char* buffer, std::streamsize size);

private:
	boost::shared_ptr<std::FILE> file;
};

} // namespace native
} // namespace filesystem
} // namespace slon

#endif // __FILESYSTEM_FILE_NATIVE_H__
