#include "stdafx.h"
#include "FileSystem/Native/File.h"
#include "Utility/error.hpp"

__DEFINE_LOGGER__("filesystem.File")

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace native {

File::File(detail::FileSystemManager*       manager,
           const boost::filesystem::path&   systemPath,
           const boost::filesystem::path&   virtualPath)
:   native::Node<filesystem::File>(manager, systemPath, virtualPath)
{
   if ( !boost::fs::is_regular_file(systemPath) ) {
       throw slon_error(logger, "File::File failed. Can't initialized file from non-file path");
   }
}

// Override Node
void File::reload()
{
}

void File::flush()
{
	if (file) fflush( file.get() );
}

// Override File
bool File::open(mask_t mode_)
{
	std::string mode;
	if ( (mode_ & in) ) {
		mode += "r";
	}
	if ( (mode_ & binary) ) {
		mode += "b";
	}

	if ( (mode_ & append) ) {
		mode += "a+";
	}
	else if ( (mode_ & truncate) ) {
		mode += "w";
	}
	else if ( (mode_ & out) || (mode_ & at_end) ) {
		mode += "a";
	}

    std::string completePath = systemPath.file_string();
	file.reset( fopen( completePath.c_str(), mode.c_str() ), fclose );
	if (!file) {
		return false;
	}

	return true;
}

void File::close()
{
	file.reset();
}

bool File::isOpen() const
{
	return (bool)file;
}

bool File::eof() const	
{
	return file ? (feof( file.get() ) != 0) : false;
}

std::streampos File::tell() const	
{
	return file ? ftell( file.get() ) : 0;
}

std::streamsize File::size() const
{
    return boost::fs::file_size(systemPath);
}

std::streampos File::seek(std::streamoff off, std::ios_base::seekdir way)
{
	if (file) 
	{
		int origin = SEEK_SET;
		if (way == std::ios_base::cur) {
			origin = SEEK_CUR;
		}
		else if (way == std::ios_base::end) {
			origin = SEEK_END;
		}
		else {
			assert( (way == std::ios_base::beg) );
		}

		return fseek(file.get(), (long)off, origin);
	}

	return 0;
}

std::streamsize File::read(char* buffer, std::streamsize size)
{
	return file ? (std::streamsize)fread( buffer, 1, size, file.get() ) : 0;
}

std::streamsize File::write(const char* buffer, std::streamsize size)
{
	return file ? (std::streamsize)fwrite( buffer, 1, size, file.get() ) : 0;
}

} // namespace native
} // namespace filesystem
} // namespace slon
