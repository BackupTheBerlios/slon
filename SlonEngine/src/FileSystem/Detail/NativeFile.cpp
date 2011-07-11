#include "stdafx.h"
#include "FileSystem/Detail/NativeFile.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("filesystem.File")

namespace boost {
	namespace fs = filesystem;
}

namespace slon {
namespace filesystem {
namespace detail {

NativeFile::NativeFile(detail::FileSystemManager*       manager,
                       const boost::filesystem::path&   systemPath,
                       const boost::filesystem::path&   virtualPath)
:   NativeNode<filesystem::File>(manager, systemPath, virtualPath)
{
   if ( !boost::fs::is_regular_file(systemPath) ) {
       throw slon_error(AUTO_LOGGER, "File::File failed. Can't initialized file from non-file path");
   }
}

// Override Node
void NativeFile::reload()
{
}

void NativeFile::flush()
{
	if (file) fflush( file.get() );
}

// Override File
bool NativeFile::open(mask_t mode_)
{
	std::string mode;
	if ( (mode_ & in) ) {
		mode += "r";
	}

	if ( (mode_ & append) ) {
		mode += "a+";
	}
	else if ( mode_ & at_end ) {
		mode += "a";
	}
	else if ( (mode_ & truncate) || (mode_ & out) ) {
		mode += "w";
	}

	if ( (mode_ & binary) ) {
		mode += "b";
	}

    std::string completePath = systemPath.file_string();
	file.reset( fopen( completePath.c_str(), mode.c_str() ), fclose );
	if (!file) {
		return false;
	}

	return true;
}

void NativeFile::close()
{
	file.reset();
}

bool NativeFile::isOpen() const
{
	return (bool)file;
}

bool NativeFile::eof() const	
{
	return file ? (feof( file.get() ) != 0) : false;
}

std::streampos NativeFile::tell() const	
{
	return file ? ftell( file.get() ) : 0;
}

std::streamsize NativeFile::size() const
{
    return boost::fs::file_size(systemPath);
}

std::streampos NativeFile::seek(std::streamoff off, std::ios_base::seekdir way)
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

std::streamsize NativeFile::read(char* buffer, std::streamsize size)
{
	return file ? (std::streamsize)fread( buffer, 1, size, file.get() ) : 0;
}

std::streamsize NativeFile::write(const char* buffer, std::streamsize size)
{
	return file ? (std::streamsize)fwrite( buffer, 1, size, file.get() ) : 0;
}

} // namespace detail
} // namespace filesystem
} // namespace slon
