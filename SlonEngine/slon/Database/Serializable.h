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
     * @return serializable name.
     * @throws serialization_error
     */
    virtual const char* serialize(OArchive& ar) const = 0;

    /** Deserialize object data using provided deserializer.
     * @param ar - archieve to read the data from.
     * @throws serialization_error
     */
    virtual void deserialize(IArchive& ar) = 0;

    virtual ~Serializable() {}
};

/** Wrapper to make object serializable. if you want serialize/deserialize object, which is not derived from Serializable
 * provide partial specialization for this class template and use OArchive::writeCustomSerializable, IArchive::readCustomSerializable.
 * Also you can specialize only serialize/deserialize functions of this class (see example below).
 * You should also register wrapper create function in DatabaseManager.
 * Example:
 * \code
 * const char* SerializableWrapper< std::pair<int,int> >::serialize(const std::pair<int,int>& pair, OArchive& ar) const
 * {
 *     ar.writeChunk("first", &pair->first);
 *     ar.writeChunk("second", &pair->second);
 *     return "int_pair";
 * }
 *     
 * T* SerializableWrapper< std::pair<int,int> >::deserialize(std::pair<int,int>*& pair, IArchive& ar)
 * {
 *     pair = new std::pair<int, int>();
 *     ar.readChunk("first", &pair->first);
 *     ar.readChunk("second", &pair->second);
 *     return pair;
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
    SerializableWrapper(const T* object_ = 0)
    :   object( const_cast<T*>(object_) )
    {}

	// Override Serializable
    const char* serialize(OArchive& ar) const
    { 
        if ( !serializeDerived(object, ar) ) {
            name = serialize(*object, ar);
        }

        return name;
    }

    void deserialize(IArchive& ar)      
    { 
        deserialize(object, ar);
    }

    /** Get serialized/deserialized object */
    T* getObject() { return object; }

    /** Serialize object. Implement this function.
     * @param object - object for serialization.
     * @param ar - archive where to write object.
     * @return serializable name.
     */
	static const char* serialize(const T& object, OArchive& ar);

    /** Deserialize object. Implement this function.
     * @param object[out] - where to store deserialized object.
     * @param ar - archive where to read from.
     * @return deserialized object.
     */
    static void deserialize(T*& object, IArchive& ar);

    /** Try to serialize derived object. Specialize this function if you
     * are about to use SerializableWrapper for polymorphic object. Specialization
     * for base SerializableWrapper should look like this:
     * \code
     * bool serializeDerived(Base* object, OArchive& ar) const
     * {
     *     return serializeDerived<Derived1>(object, ar)
     *            || serializeDerived<Derived2>(object, ar)
     *            || serializeDerived<Derived3>(object, ar);
     * }
     * \uncode
     */
    bool serializeDerived(T* object, OArchive& ar) const
    {
        return false;
    }

    /** Try to serialize using dervied object wrapper. */
    template<typename Y>
    bool serializeDerived(T* object, OArchive& ar) const
    {
        if ( Y* derived = dynamic_cast<Y*>(object) ) 
        {
            SerializableWrapper<Y> wrapper(derived);
            name = wrapper.serialize(ar);
            return true;
        }

        return false;
    }

    /// Implement this if you want custom wrapper create function
	static SerializableWrapper<T>* createWrapper()
	{
		return new SerializableWrapper<T>();
	}

private:
    T* object;
    mutable const char* name;
};


/** Helper function for registering serializable. Example:
 * \code
 * database::currentDatabaseManager().registerSerializableCreateFunc("Node, createSerializable<scene::Node>);
 * \uncode
 */
template<typename T>
database::Serializable* createSerializable()
{
	return new T();
}

/** Helper function for registering serializable wrappers. Example:
 * \code
 * database::currentDatabaseManager().registerSerializableCreateFunc("int_pair, createSerializableWrapper< std::pair<int, int> >);
 * \uncode
 */
template<typename T>
inline SerializableWrapper<T>* createSerializableWrapper()
{
	return new SerializableWrapper<T>();
}

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_SERIALIZABLE_H__