#ifndef __SLON_ENGINE_DATABASE_SERIALIZABLE_H__
#define __SLON_ENGINE_DATABASE_SERIALIZABLE_H__

#include "Forward.h"

namespace slon {
namespace database {

/** construct the full version from major, minor and patch version
 * @param major - major version (up to 8 bytes)
 * @param minor - minor version (up to 12 bytes)
 * @param patch - patch version (up to 12 bytes)
 */
inline unsigned getVersion(unsigned major, unsigned minor, unsigned patch)
{
    return ((major << 24) & 0xFF000000) | ((minor << 12) & 0x00FFF000) | (patch & 0x00000FFF); 
}

/** Get major version from full version. */
inline unsigned getMajorVersion(unsigned version)
{
    return (version & 0xFF000000) >> 24; 
}

/** Get minor version from full version. */
inline unsigned getMinorVersion(unsigned version)
{
    return (version & 0x00FFF000) >> 12; 
}

/** Get patch version from full version. */
inline unsigned getPatchVersion(unsigned version)
{
    return (version & 0x00000FFF); 
}

/** Abstract base class for objects serializable/deserializable using slon serializers */
class Serializable
{
public:
    /** Get name of the class for serialization. */
    virtual const char* getSerializableName() const = 0;

    /** Serialize object data using provided serializer.
     * @param ar - archive where to write data. 
     * @param version - file format version.
     * @throws serialization_error
     */
    virtual void serialize(OArchive& ar) const = 0;

    /** Deserialize object data using provided deserializer.
     * @param ar - archieve to read the data from.
     * @param version - file format version.
     * @throws serialization_error
     */
    virtual void deserialize(IArchive& ar) = 0;

    virtual ~Serializable() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_SERIALIZABLE_H__