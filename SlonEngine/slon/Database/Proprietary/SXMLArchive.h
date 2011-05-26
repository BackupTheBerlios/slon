#ifndef __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_ARCHIVE_H__
#define __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_ARCHIVE_H__

#include "../../Filesystem/Forward.h"
#include "../Archive.h"
#include <xml++/document.h>
#include <xml++/element.h>

namespace slon {
namespace database {

class SXMLIArchive :
	public IArchive
{
private:
    typedef std::map<int, xmlpp::element_iterator>	reference_element_map;
    typedef std::map<int, Serializable*>			reference_serializable_map;

public:
	SXMLIArchive();
	
	/** Read archive content from file */
	void readFromFile(filesystem::File& file);

    // Override Archive
    unsigned getVersion() const { return version; }

    // Override IArchive
    Serializable* readReference(int refId);
	Serializable* readSerializableOrReference();

    bool openChunk(const char* name, chunk_info& info);
    void closeChunk();

	void readString(char* str)		{ memcpy(str, openedElement->get_text(), chunkInfo.size); }
    void readString(wchar_t* str)	{ /*memcpy(str, currentElement.get_text()); FIXME*/ }
    void read(boolean* values)		{ readImpl(values); }
    void read(int8* values)			{ readImpl(values); }
    void read(uint8* values)		{ readImpl(values); }
    void read(int16* values)		{ readImpl(values); }
    void read(uint16* values)		{ readImpl(values); }
    void read(int32* values)		{ readImpl(values); }
    void read(uint32* values)		{ readImpl(values); }
    void read(int64* values)		{ readImpl(values); }
    void read(uint64* values)		{ readImpl(values); }
    void read(float32* values)		{ readImpl(values); }
	void read(float64* values)		{ readImpl(values); }
	
    template<typename T>
    void readImpl(T* values)
    {
		if (!openedElement) {
			throw serialization_error(log::logger_ptr(), "Trying to read raw data from unopened chunk.");
		}
        else if (!chunkInfo.isLeaf) {
			throw serialization_error(log::logger_ptr(), "Trying to read raw data from not leaf chunk.");
		}

        std::istringstream ss( openedElement->get_text() );
        for (size_t i = 0; i<chunkInfo.size; ++i) {
            ss >> values[i];
		}
    }

private:
	void			readChunkInfo();
	Serializable*	readSerializable(xmlpp::element& el);

private:
    unsigned					version;
    reference_serializable_map  referenceSerializables;
	reference_element_map		referenceElements;

    xmlpp::document			document;
	xmlpp::element_iterator openedElement;
	xmlpp::element_iterator nextElement;
	chunk_info				chunkInfo;
};

class SXMLOArchive :
    public OArchive
{
private:
    typedef std::map<Serializable*, int> reference_map;

public:
    SXMLOArchive(unsigned version);

    // Override Archive
    unsigned getVersion() const { return version; }

    // Override OArchive
    int registerReference(Serializable* serializable);
    int getReferenceId(Serializable* serializable) const;

    void openChunk(const char* name);
    void closeChunk();

	void writeSerializablOrReference(Serializable* serializable);
    void writeReferenceChunk(int refId);
    void writeStringChunk(const char* name, const char* str, size_t size);
    void writeStringChunk(const char* name, const wchar_t* str, size_t size);
	
    void writeChunk(const char* name, const boolean* values, size_t count = 1)	{ writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const int8* values, size_t count = 1)     { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const uint8* values, size_t count = 1)    { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const int16* values, size_t count = 1)    { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const uint16* values, size_t count = 1)   { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const int32* values, size_t count = 1)    { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const uint32* values, size_t count = 1)   { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const int64* values, size_t count = 1)    { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const uint64* values, size_t count = 1)   { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const float32* values, size_t count = 1)  { writeChunkImpl(name, values, count); }
    void writeChunk(const char* name, const float64* values, size_t count = 1)  { writeChunkImpl(name, values, count); }

    template<typename T>
    void writeChunkImpl(const char* name, const T* values, size_t count)
    {
        xmlpp::element child(name);

        std::ostringstream ss;
        for (size_t i = 0; i<count; ++i) 
		{
            ss << values[i];
			if (i < count - 1) {
				ss << ' ';
			}
		}

        child.set_text( ss.str().c_str() );
        currentElement.add_child(child);
    }

    void writeToFile(filesystem::File& file) const;

private:
    unsigned        version;
    reference_map   references;

    xmlpp::document document;
    xmlpp::element  currentElement;
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_ARCHIVE_H__
