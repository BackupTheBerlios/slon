#ifndef __SLON_ENGINE_GRAPHICS_MESH_CONSTRUCTOR_H__
#define __SLON_ENGINE_GRAPHICS_MESH_CONSTRUCTOR_H__

#include "Mesh.h"
#include <boost/scoped_array.hpp>
#include <sgl/Types.h>
#include <string>

namespace slon {
namespace graphics {
	
/** Helper class for constructing meshes. */
class MeshConstructor :
    public Referenced
{
public:
    static const int MAX_NUM_ATTRIBUTES = 16;

    /// Array of the attributes with information about them
    struct attribute_array
    {
        std::string                 name;  /// name of the attribute, also defines semantic
        unsigned                    size;  /// number of elements in the attribute
        unsigned                    count; /// number of attributes in the array
        sgl::SCALAR_TYPE            type;  /// type of the attribute elements
        boost::scoped_array<char>   data;  /// attribute array

        attribute_array(unsigned            _size   = 0,
                        unsigned            _count  = 0,
                        sgl::SCALAR_TYPE    _type   = sgl::UNKNOWN )
        :   size(_size)
        ,   count(_count)
        ,   type(_type)
        {}

        size_t attribute_size() const
        {
            return size * sgl::SCALAR_TYPE_TRAITS[type].sizeInBits / 8;
        }
    };

    /// Array of the indices for attributes
    struct indices_array
    {
        unsigned                        count;   /// number of indices in the array
        boost::scoped_array<unsigned>   indices; /// indices array

        indices_array(unsigned _count = 0) :
            count(_count)
        {}
    };

public:
	/** Get attributes array by index */
	const attribute_array& getAttributes(unsigned index) const;
	
	/** Get indices array by index */
	const indices_array& getIndices(unsigned index) const;

    /** Append attribute array to the vertex array.
     * @param index - attribute index.
     * @param size - number of attribute components.
     * @param count - number of attributes in the array.
     * @param type - type of the attribute component.
     * @param data - attribute data.
     */
    void setAttributes( const std::string&  name,
                        unsigned            index,
                        unsigned            size,
                        unsigned            count,
                        sgl::SCALAR_TYPE    type,
                        const void*         data );

    /** Setup indices of attributes.
     * @param attribute - attribute for indexing.
     * @param indices - indices for attribute.
     */
    void setIndices( unsigned           attribute,
                     unsigned           numIndices,
                     const unsigned*    indices );

    /** Get number of specified attributes. */
    unsigned getAttributeCount(unsigned attribute);

    /** Try convert attribute array to specified type. */
    template<typename AttributeType>
    const AttributeType* queryAttributeData(unsigned attribute) const;

    /** Create mesh */
    mesh_ptr createMesh() const;

private:
    attribute_array     attributeArrays[MAX_NUM_ATTRIBUTES];
    indices_array       indicesArrays[MAX_NUM_ATTRIBUTES];
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_MESH_CONSTRUCTOR_H__
