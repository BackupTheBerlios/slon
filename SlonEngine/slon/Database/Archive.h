#ifndef __SLON_ENGINE_DATABASE_ARCHIVE_H__
#define __SLON_ENGINE_DATABASE_ARCHIVE_H__

#include "../Utility/error.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace database {

/** Abstract base class for archieves - classes which control how data should be read or written to the file (or other media). */
class Archive :
    public Referenced
{
public:
    // archive types
	typedef bool				boolean;

    typedef char                int8;
    typedef unsigned char       uint8;

    typedef short               int16;
    typedef unsigned short      uint16;

    typedef int                 int32;
    typedef unsigned int        uint32;

    typedef long long           int64;
    typedef unsigned long long  uint64;

    typedef float               float32;
    typedef double              float64;

public:
    /** Get version of the format for serialization. */
    virtual unsigned getVersion() const = 0;

    virtual ~Archive() {}
};

/** Base class for input archives. */
class IArchive :
    public Archive
{
public:
    struct chunk_info
    {
        int     refId;  /// reference id of the chunk, 0 - if chunk is not reference
        size_t  size;   /// number of array elements in the chunk if chunk is leaf, otherwise number of sub chunks
        bool    isLeaf; /// chunk doesn't have sub chunks
    };

public:
    /** Read serializable using chunk reference id (or return pointer to it if it has been already read). 
     * @param refId - chunk reference id which we want to read from.
     * @return pointer serializable or NULL pointer if chunk with specified reference id was not found.
     */
    virtual Serializable* readReference(int refId) = 0;

	/** Read serializable from the next chunk. */
	virtual Serializable* readSerializableOrReference() = 0;

    /** Open sub chunk.
     * @param name - name of the sub chunk to open.
     * @param info [out] - information about read chunk.
     * @return true on success.
     */
    virtual bool openChunk(const char* name, chunk_info& info) = 0;

    /** Close currently opened chunk. */
    virtual void closeChunk() = 0;

    /** Read string from the chunk. */
    virtual void readString(char* str) = 0;

    /** Read string from the chunk. */
    virtual void readString(wchar_t* str) = 0;
	
    /** Read value array from the chunk. */
    virtual void read(boolean* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(int8* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(uint8* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(int16* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(uint16* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(int32* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(uint32* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(int64* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(uint64* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(float32* values) = 0;

    /** Read value array from the chunk. */
    virtual void read(float64* values) = 0;

    virtual ~IArchive() {}
};

/** Base class for output archives. */
class OArchive :
    public Archive
{
public:
    /** Register serializable in the archive.
     * @param serializable - serializable for registration.
     * @return 0 - if serializable already registered, otherwise registers serializable and returns unique reference id.
     */
    virtual int registerReference(Serializable* serializable) = 0;

    /** Get reference id of the registered serializable.
     * @param serializable - serializable which reference id we want to retrieve.
     * @return reference id of the serializable or 0 if serializable id not registered.
     */
    virtual int getReferenceId(Serializable* serializable) const = 0;

    /** Open chunk for writing subchunks.
     * @param name - name of the chunk.
     */
    virtual void openChunk(const char* name) = 0;

    /** Close currently opened chunk. */
    virtual void closeChunk() = 0;

	/** Write serializable, or reference if it already serialized. */
	virtual void writeSerializablOrReference(Serializable* serializable) = 0;

    /** Write reference chunk. */
    virtual void writeReferenceChunk(int refId) = 0;

    /** Make leaf chunk and write string into it.
     * @param name - name of the chunk.
     * @param str - string to write into chunk.
     * @param size - length of the string.
     */
    virtual void writeStringChunk(const char* name, const char* str, size_t size) = 0;

    /** Make leaf chunk and write string into it.
     * @param name - name of the chunk.
     * @param str - string to write into chunk.
     * @param size - length of the string.
     */
    virtual void writeStringChunk(const char* name, const wchar_t* str, size_t size) = 0;
	
    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const boolean* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const int8* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const uint8* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const int16* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const uint16* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const int32* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const uint32* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const int64* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const uint64* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const float32* values, size_t count = 1) = 0;

    /** Make leaf chunk and write array into it.
     * @param name - name of the chunk.
     * @param values - array of the values for writing.
     * @param count - number of elements in the array.
     */
    virtual void writeChunk(const char* name, const float64* values, size_t count = 1) = 0;

    virtual ~OArchive() {}
};

inline std::string readStringChunk(IArchive& ar, const std::string& name)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	
	std::string str(info.size, ' ');
	ar.readString(&str[0]);
	ar.closeChunk();
	
	return str;
}

inline std::wstring readWStringChunk(IArchive& ar, const std::string& name)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	
	std::wstring str(info.size, ' ');
	ar.readString(&str[0]);
	ar.closeChunk();
	
	return str;
}

template<typename Char, typename Traits>
inline void readStringChunk(IArchive& ar, const std::string& name, std::basic_string<Char, Traits>& str)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	
	str.resize(info.size);
	ar.readString(&str[0]);
	ar.closeChunk();
	
	return str;
}

template<typename T>
inline T readChunk(IArchive& ar, const std::string& name)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	else if (info.size != 1) {
		throw serialization_error("Trying to read single item from multi-item chunk");
	}
	
	T value;
	ar.read(&value);
	ar.closeChunk();
	
	return value;
}

template<typename T>
inline void readChunk(IArchive& ar, const std::string& name, T* value)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	else if (info.size != 1) {
		throw serialization_error("Trying to read single item from multi-item chunk");
	}
	
	ar.read(value);
	ar.closeChunk();
}

template<typename T>
inline void readChunk(IArchive& ar, const std::string& name, T* data, size_t numElements)
{
	IArchive::chunk_info info;
	if ( !ar.openChunk(name.c_str(), info) ) {
		throw serialization_error("Can't open requested chunk");
	}
	else if (!info.isLeaf) {
		throw serialization_error("Trying to read raw data from non leaf chunk");
	}
	else if (info.size != numElements) {
		throw serialization_error("Number of elements in chunk is not equal to requested number of elements");
	}
	
	ar.read(data);
	ar.closeChunk();
}

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_ARCHIVE_H__
