#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Detail/AttributeTable.h"
#include "Graphics/MeshConstructor.h"
#include "Log/Logger.h"
#include "Utility/math.hpp"

DECLARE_AUTO_LOGGER("graphics.MeshConstructor")

namespace slon {
namespace graphics {

#define QUERY_ATTRIBUTE_DATA(Type, SglType, Size)\
template<>\
const Type* MeshConstructor::queryAttributeData<Type>(unsigned attribute) const\
{\
    assert(attribute < MAX_NUM_ATTRIBUTES);\
    if (attributeArrays[attribute].count > 0\
        && attributeArrays[attribute].type == SglType\
        && attributeArrays[attribute].size == Size)\
    {\
        return reinterpret_cast<const Type*>( attributeArrays[attribute].data.get() );\
    }\
    return 0;\
}

QUERY_ATTRIBUTE_DATA(int,               sgl::INT,       1)
QUERY_ATTRIBUTE_DATA(math::Vector2i,    sgl::INT,       2)
QUERY_ATTRIBUTE_DATA(math::Vector3i,    sgl::INT,       3)
QUERY_ATTRIBUTE_DATA(math::Vector4i,    sgl::INT,       4)

QUERY_ATTRIBUTE_DATA(float,             sgl::FLOAT,     1)
QUERY_ATTRIBUTE_DATA(math::Vector2f,    sgl::FLOAT,     2)
QUERY_ATTRIBUTE_DATA(math::Vector3f,    sgl::FLOAT,     3)
QUERY_ATTRIBUTE_DATA(math::Vector4f,    sgl::FLOAT,     4)
/*
QUERY_ATTRIBUTE_DATA(double,            sgl::DOUBLE,    1)
QUERY_ATTRIBUTE_DATA(math::Vector2d,    sgl::DOUBLE,    2)
QUERY_ATTRIBUTE_DATA(math::Vector2d,    sgl::DOUBLE,    3)
QUERY_ATTRIBUTE_DATA(math::Vector2d,    sgl::DOUBLE,    4)
*/
#undef QUERY_ATTRIBUTE_DATA

unsigned MeshConstructor::getAttributeCount(unsigned attribute)
{
    assert(attribute < MAX_NUM_ATTRIBUTES);
    return attributeArrays[attribute].count;
}

const MeshConstructor::attribute_array& MeshConstructor::getAttributes(unsigned index) const
{
    assert(index < MAX_NUM_ATTRIBUTES);
	return attributeArrays[index];
}

const MeshConstructor::indices_array& MeshConstructor::getIndices(unsigned index) const
{
    assert(index < MAX_NUM_ATTRIBUTES);
	return indicesArrays[index];
}

void MeshConstructor::setAttributes( const std::string&  name,
                                     unsigned            index,
                                     unsigned            size,
                                     unsigned            count,
                                     sgl::SCALAR_TYPE    type,
                                     const void*         data )
{
    assert(index < MAX_NUM_ATTRIBUTES);

    // copy data
    if (data)
    {
        size_t dataSize = size * count * sgl::SCALAR_TYPE_TRAITS[type].sizeInBits / 8;
        attributeArrays[index].data.reset(new char[dataSize]);
        memcpy(attributeArrays[index].data.get(), data, dataSize);
    }

    // create attribute array
    attributeArrays[index].name  = name;
    attributeArrays[index].type  = type;
    attributeArrays[index].size  = size;
    attributeArrays[index].count = count;
}

void MeshConstructor::setIndices( unsigned           attribute,
                                  unsigned           count,
                                  const unsigned*    indices )
{
    assert(attribute < MAX_NUM_ATTRIBUTES);

    // create indices array
    indicesArrays[attribute].indices.reset(new unsigned[count]);
    indicesArrays[attribute].count = count;
    std::copy( indices, indices + count, indicesArrays[attribute].indices.get() );
}

mesh_ptr MeshConstructor::createMesh() const
{
    Mesh::DESC desc;
	assert(attributeArrays[0].count > 0 && "constructor should have zero(position) attribute");

    // calculate size of the buffer
	desc.vertexSize = 0;
    for(int i = 0; i<MAX_NUM_ATTRIBUTES; ++i) {
		desc.vertexSize += attributeArrays[i].attribute_size();
    }

	// arrays to store new data
    std::vector<char>       attributeData( desc.vertexSize * attributeArrays[0].count );
	int                     attrIndices[MAX_NUM_ATTRIBUTES];
    std::vector<int>        attributesPlaced[MAX_NUM_ATTRIBUTES];
	std::vector<unsigned>   indices( indicesArrays[0].count );

	for (int i = 0; i<MAX_NUM_ATTRIBUTES; ++i)
    {
        attributesPlaced[i].resize(attributeArrays[0].count);
        std::fill( attributesPlaced[i].begin(), attributesPlaced[i].end(), -1 );
	}

	// merge
    for (size_t i = 0; i<indicesArrays[0].count; ++i)
    {
		// get attr indices
		for(int j = 0; j<MAX_NUM_ATTRIBUTES; ++j)
        {
            if ( indicesArrays[j].count > size_t(j) ) {
			    attrIndices[j] = indicesArrays[j].indices[i];
            }
            else {
                attrIndices[j] = indicesArrays[0].indices[i];
            }
		}
		size_t index = attrIndices[0];

		// check when need to copy vertex
		bool makeVertex = false;
		for(int j = 0; j<MAX_NUM_ATTRIBUTES; ++j)
		{
			if ( attributesPlaced[j][index] > 0
				 && attributesPlaced[j][index] != attrIndices[j] )
			{
				makeVertex = true;
				break;
			}
		}

		// add new vertex to the vbo
		if (makeVertex)
		{
			index = attributeData.size() / desc.vertexSize;
            attributeData.resize(attributeData.size() + desc.vertexSize);

            size_t stride = 0;
            for(int j = 0; j<MAX_NUM_ATTRIBUTES; ++j)
			{
                size_t attributeSize = attributeArrays[j].attribute_size();
                if (attributeSize > 0)
                {
				    attributesPlaced[j].push_back( attrIndices[j] );

				    memcpy( &attributeData[index * desc.vertexSize + stride],
						    &attributeArrays[j].data[attrIndices[j] * attributeSize],
                            attributeArrays[j].attribute_size() );

                    stride += attributeSize;
                }
			}
		}
		else
		{
			// copy vertex
            size_t stride = 0;
			for(int j = 0; j<MAX_NUM_ATTRIBUTES; ++j)
			{
                size_t attributeSize = attributeArrays[j].attribute_size();
                if (attributeSize > 0)
                {
				    attributesPlaced[j][index] = attrIndices[j];

				    memcpy( &attributeData[index * desc.vertexSize + stride],
						    &attributeArrays[j].data[attrIndices[j] * attributeSize],
                            attributeArrays[j].attribute_size() );

                    stride += attributeSize;
                }
			}
		}

		indices[i] = index;
	}

	// create vbo & ibo
    sgl::Device* device = graphics::currentDevice();
    desc.vertexBuffer.reset( device->CreateVertexBuffer() );
    desc.vertexBuffer->SetData(attributeData.size(), &attributeData[0]);
	
    desc.indexType = sgl::IndexBuffer::UINT_32;
    desc.indexBuffer.reset( device->CreateIndexBuffer() );
    {
        // copy indices
        size_t count = attributeData.size() / desc.vertexSize;
        if ( count <= std::numeric_limits<unsigned short>::max() )
        {
            std::vector<unsigned short> shortIndices( indices.begin(), indices.end() );
            desc.indexBuffer->SetData(shortIndices.size() * sizeof(unsigned short), &shortIndices[0]);
            desc.indexType = sgl::IndexBuffer::UINT_16;
        }
        else
        {
            desc.indexBuffer->SetData(indices.size() * sizeof(unsigned int), &indices[0]);
            desc.indexType = sgl::IndexBuffer::UINT_32;
        }
    }

    // calc aabb
    desc.aabb = bounds<math::AABBf>::inv_infinite();
    {
        if ( const math::Vector3f* vertices = queryAttributeData<math::Vector3f>(0) )
        {
            for (size_t i = 0; i<attributeArrays[0].count; ++i) {
                desc.aabb.extend(vertices[i]);
            }
        }
        else if ( const math::Vector4f* vertices = queryAttributeData<math::Vector4f>(0) )
        {
            for (size_t i = 0; i<attributeArrays[0].count; ++i) {
                desc.aabb.extend( math::xyz(vertices[i]) );
            }
        }
    }

    // list attributes
    std::vector<Mesh::attribute> attributes;
    {
        size_t offset = 0;
        for (int i = 0; i<MAX_NUM_ATTRIBUTES; ++i)
        {
            if (attributeArrays[i].count > 0)
            {
                Mesh::attribute attribute;
                {
                    attribute.binding   = detail::currentAttributeTable().queryAttribute( hash_string(attributeArrays[i].name) );
                    attribute.size      = attributeArrays[i].size;
                    attribute.type      = attributeArrays[i].type;
                    attribute.offset    = offset;

                    if (attributeArrays[i].name == "position")          attribute.semantic = Mesh::POSITION;
                    else if (attributeArrays[i].name == "normal")       attribute.semantic = Mesh::NORMAL;
                    else if (attributeArrays[i].name == "tangent")      attribute.semantic = Mesh::TANGENT;
                    else if (attributeArrays[i].name == "color")        attribute.semantic = Mesh::COLOR;
                    else if (attributeArrays[i].name == "texcoord")     attribute.semantic = Mesh::TEXCOORD;
                    else if (attributeArrays[i].name == "bone_index")   attribute.semantic = Mesh::BONE_INDEX;
                    else if (attributeArrays[i].name == "bone_weight")  attribute.semantic = Mesh::BONE_WEIGHT;
                    else attribute.semantic = Mesh::ATTRIBUTE;
                }
                attributes.push_back(attribute);

                offset += attributeArrays[i].attribute_size();
            }
        }
    }
    desc.numAttributes = attributes.size();
    desc.attributes    = &attributes[0];

    return mesh_ptr( new Mesh(desc) );
}


} // namespace graphics
} // namespace slon
