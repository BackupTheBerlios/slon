#ifndef __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_ARCHIVE_H__
#define __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_ARCHIVE_H__

#include "../../Filesystem/Forward.h"
#include "../Archive.h"
#include <xml++/document.h>
#include <xml++/element.h>

namespace slon {
namespace database {

class SXMLOArchive :
    public OArchive
{
private:
    typedef std::map<Serializable*, int> reference_map;

public:
    SXMLOArchive(unsigned version);

    /** Get output document */
    xmlpp::document& getDocument() { return document; }

    // Override Archive
    unsigned getVersion() const { return version; }

    // Override OArchive
    int registerReference(Serializable* serializable);
    int getReferenceId(Serializable* serializable) const;

    void openChunk(const char* name);
    void closeChunk();

    void writeReferenceChunk(int refId);
    void writeStringChunk(const char* name, const char* str, size_t size);
    void writeStringChunk(const char* name, const wchar_t* str, size_t size);

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
        for (size_t i = 0; i<count; ++i) {
            ss << values[i] << " ";
        }

        child.set_text(ss);
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
