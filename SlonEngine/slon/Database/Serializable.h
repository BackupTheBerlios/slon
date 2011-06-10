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

class SerializableTraits
{
public:
	/** Get name of the object for serialization. Specialize it if you want custom name for serializable. */
	static const char* getName() { return typeid(T).name(); }
};

/** Wrapper to make object serializable. if you want serialize/deserialize object, which is not derived from Serializable
 * provide partial specialization for this class template and use OArchive::writeCustomSerializable, IArchive::readCustomSerializable.
 * Also you can specialize only serialize/deserialize functions of this class (see example below).
 * You should also register wrapper create function in DatabaseManager.
 * Example:
 * \code
 * void SerializableWrapper< std::pair<int,int> >::serialize(OArchive& ar) const
 * {
 *     ar.writeChunk("first", &obj->first);
 *     ar.writeChunk("second", &obj->second);
 * }
 *     
 * void SerializableWrapper< std::pair<int,int> >::deserialize(IArchive& ar)
 * {
 *     obj = new std::pair<int, int>();
 *     ar.readChunk("first", &obj->first);
 *     ar.readChunk("second", &obj->second);
 * }
 * 
 * int main(void)
 * {
 *     REGISTER_SERIALIZABLE_WRAPPER(std::pair<int, int>);
 * }
 * \uncode
 */
template<typename T>
class SerializableWrapper :
	public Serializable
{
public:
	SerializableWrapper(const T* obj_ = 0)
	:	obj(const_cast<T*>(obj_))
	{}
	
	// Override Serializable
	void serialize(OArchive& ar) const;
    void deserialize(IArchive& ar);

	T* getObject() 
	{ 
		return obj; 
	}
		
	static SerializableWrapper<T>* createWrapper()
	{
		return new SerializableWrapper<T>();
	}

private:
	T* obj;
};

#define REGISTER_SERIALIZABLE_WRAPPER(Type)\
slon::database::currentDatabaseManager().registerSerializableCreateFunc(SerializableTraits<Type >::getName(), slon::database::SerializableWrapper<Type >::createWrapper())

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_SERIALIZABLE_H__