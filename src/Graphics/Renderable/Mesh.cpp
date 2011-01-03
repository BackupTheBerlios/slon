#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Renderable/Mesh.h"
#include "Graphics/Renderer.h"
#include "Utility/error.hpp"
#include "Utility/math.hpp"

__DEFINE_LOGGER__("graphics.Mesh")

namespace 
{
    sgl::VertexLayout::SEMANTIC BIND_SEMANTIC[] = 
    {
        sgl::VertexLayout::VERTEX,
		sgl::VertexLayout::NORMAL,
        sgl::VertexLayout::ATTRIBUTE,
		sgl::VertexLayout::COLOR,
        sgl::VertexLayout::ATTRIBUTE,
        sgl::VertexLayout::ATTRIBUTE,
        sgl::VertexLayout::ATTRIBUTE,
		sgl::VertexLayout::TEXCOORD
    };

} // anonymous namespace


namespace slon {
namespace graphics {

#define QUERY_ATTRIBUTE_DATA(Type, SglType, Size)\
template<>\
const Type* MeshData::queryAttributeData<Type>(unsigned attribute) const\
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

unsigned MeshData::getcount(unsigned attribute)
{
    assert(attribute < MAX_NUM_ATTRIBUTES);
    return attributeArrays[attribute].count;
}

const MeshData::attribute_array& MeshData::getAttributes(unsigned index) const
{
    assert(index < MAX_NUM_ATTRIBUTES);
	return attributeArrays[index];
}

const MeshData::indices_array& MeshData::getIndices(unsigned index) const
{
    assert(index < MAX_NUM_ATTRIBUTES);
	return indicesArrays[index];
}

void MeshData::setAttributes( const std::string&  name,
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
        char*  attrData = (char*)malloc(dataSize);
        if (!attrData) {
            throw std::bad_alloc();
        }
        memcpy(attrData, data, dataSize);
        attributeArrays[index].data.reset(attrData, free);
    }

    // create attrubute array
    attributeArrays[index].name     = name;
    attributeArrays[index].type     = type;
    attributeArrays[index].size     = size;
    attributeArrays[index].count    = count;
}

void MeshData::setIndices( unsigned           attribute,
                           unsigned           count,
                           const unsigned*    indices )
{
    assert(attribute < MAX_NUM_ATTRIBUTES);

    // create indices array
    indicesArrays[attribute].indices.reset(new unsigned[count]);
    indicesArrays[attribute].count = count;
    std::copy( indices, indices + count, indicesArrays[attribute].indices.get() );
}

Mesh::buffer_lock_impl::buffer_lock_impl(sgl::Buffer* buffer_,
                                         const int    mask) :
    buffer(buffer_)
{
    assert( buffer && !buffer->Mapped() );
    if ( sgl::SGL_OK != buffer->Map(mask, &ptr) ) {
        throw slon_error(logger, "Can't lock buffer");
    }
}

Mesh::buffer_lock_impl::~buffer_lock_impl()
{
    buffer->Unmap();
}

Mesh::plain_subset* Mesh::addPlainSubset( Effect*             effect,
                                          sgl::PRIMITIVE_TYPE primitiveType,
                                          unsigned            startVertex,
                                          unsigned            numVertices )
{
    plain_subset* subset = new plain_subset(this, effect, primitiveType, startVertex, numVertices);
    subsets.push_back( subset_ptr(subset) );
    return subset;
}

Mesh::indexed_subset* Mesh::addIndexedSubset( Effect*             effect,
                                              sgl::PRIMITIVE_TYPE primitiveType,
                                              unsigned            startIndex,
                                              unsigned            numIndices )
{
    indexed_subset* subset = new indexed_subset(this, effect, primitiveType, startIndex, numIndices);
    subsets.push_back( subset_ptr(subset) );
    return subset;
}

void Mesh::plain_subset::render() const
{
    mesh->vertexBuffer->Bind( mesh->vertexLayout.get() );
    currentDevice()->Draw(primitiveType, startVertex, numVertices);
}

void Mesh::indexed_subset::render() const
{
    mesh->vertexBuffer->Bind( mesh->vertexLayout.get() );
    mesh->indexBuffer->Bind( mesh->indexType );
    currentDevice()->DrawIndexed(primitiveType, startIndex, numIndices);
}

Mesh::Mesh(const MeshData* data_)
:	data(data_)
{
    // attributes must have zero attribute
    assert(data && "data may not be NULL");
	assert(data->attributeArrays[0].count > 0 && "mesh must have zero attribute");

    // calculate size of the buffer
	vertexSize = 0;
    for(int i = 0; i<MeshData::MAX_NUM_ATTRIBUTES; ++i) {
		vertexSize += data->attributeArrays[i].attribute_size();
    }

	// arrays to store new data
    std::vector<char>       attributeData( vertexSize * data->attributeArrays[0].count );
	int                     attrIndices[MeshData::MAX_NUM_ATTRIBUTES];
    std::vector<int>        attributesPlaced[MeshData::MAX_NUM_ATTRIBUTES];
	std::vector<unsigned>   indices( data->indicesArrays[0].count );

	for (int i = 0; i<MeshData::MAX_NUM_ATTRIBUTES; ++i)
    {
        attributesPlaced[i].resize(data->attributeArrays[0].count);
        std::fill( attributesPlaced[i].begin(), attributesPlaced[i].end(), -1 );
	}

	// merge
    for (size_t i = 0; i<data->indicesArrays[0].count; ++i)
    {
		// get attr indices
		for(int j = 0; j<MeshData::MAX_NUM_ATTRIBUTES; ++j)
        {
            if ( data->indicesArrays[j].count > size_t(j) ) {
			    attrIndices[j] = data->indicesArrays[j].indices[i];
            }
            else {
                attrIndices[j] = data->indicesArrays[0].indices[i];
            }
		}
		size_t index = attrIndices[0];

		// check when need to copy vertex
		bool makeVertex = false;
		for(int j = 0; j<MeshData::MAX_NUM_ATTRIBUTES; ++j)
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
			index = attributeData.size() / vertexSize;
            attributeData.resize(attributeData.size() + vertexSize);

            size_t stride = 0;
            for(int j = 0; j<MeshData::MAX_NUM_ATTRIBUTES; ++j)
			{
                size_t attributeSize = data->attributeArrays[j].attribute_size();
                if (attributeSize > 0)
                {
				    attributesPlaced[j].push_back( attrIndices[j] );

				    memcpy( &attributeData[index * vertexSize + stride],
						    &data->attributeArrays[j].data[attrIndices[j] * attributeSize],
                            data->attributeArrays[j].attribute_size() );

                    stride += attributeSize;
                }
			}
		}
		else
		{
			// copy vertex
            size_t stride = 0;
			for(int j = 0; j<MeshData::MAX_NUM_ATTRIBUTES; ++j)
			{
                size_t attributeSize = data->attributeArrays[j].attribute_size();
                if (attributeSize > 0)
                {
				    attributesPlaced[j][index] = attrIndices[j];

				    memcpy( &attributeData[index * vertexSize + stride],
						    &data->attributeArrays[j].data[attrIndices[j] * attributeSize],
                            data->attributeArrays[j].attribute_size() );

                    stride += attributeSize;
                }
			}
		}

		indices[i] = index;
	}

	// create vbo & ibo
    sgl::Device* device = graphics::currentDevice();
    vertexBuffer.reset( device->CreateVertexBuffer() );
    vertexBuffer->SetData(attributeData.size(), &attributeData[0]);
	
    indexType = sgl::IndexBuffer::UINT_32;
    indexBuffer.reset( device->CreateIndexBuffer() );
    {
        // copy indices
        size_t count = attributeData.size() / vertexSize;
        /*
        if ( count <= std::numeric_limits<unsigned char>::max() )
        {
            std::vector<unsigned char> charIndices( indices.begin(), indices.end() );
            ibo->SetData(charIndices.size() * sizeof(unsigned char), &charIndices[0]);
            indexType = sgl::IndexBuffer::UINT_8;
        }
        else
        */
        if ( count <= std::numeric_limits<unsigned short>::max() )
        {
            std::vector<unsigned short> shortIndices( indices.begin(), indices.end() );
            indexBuffer->SetData(shortIndices.size() * sizeof(unsigned short), &shortIndices[0]);
            indexType = sgl::IndexBuffer::UINT_16;
        }
        else
        {
            indexBuffer->SetData(indices.size() * sizeof(unsigned int), &indices[0]);
            indexType = sgl::IndexBuffer::UINT_32;
        }
    }

    // calc aabb
    aabb = bounds<math::AABBf>::inv_infinite();
    {
        if ( const math::Vector3f* vertices = data->queryAttributeData<math::Vector3f>(0) )
        {
            for (size_t i = 0; i<data->attributeArrays[0].count; ++i) {
                aabb.extend(vertices[i]);
            }
        }
        else if ( const math::Vector4f* vertices = data->queryAttributeData<math::Vector4f>(0) )
        {
            for (size_t i = 0; i<data->attributeArrays[0].count; ++i) {
                aabb.extend( math::xyz(vertices[i]) );
            }
        }
    }

    // list attributes
    {
        size_t offset = 0;
        for (int i = 0; i<MeshData::MAX_NUM_ATTRIBUTES; ++i)
        {
            if (data->attributeArrays[i].count > 0)
            {
                Mesh::attribute attribute;
                attribute.binding   = detail::currentAttributeTable().queryAttribute( unique_string(data->attributeArrays[i].name) );
                attribute.size      = data->attributeArrays[i].size;
                attribute.type      = data->attributeArrays[i].type;
                attribute.offset    = offset;

                if (data->attributeArrays[i].name == "position")          attribute.semantic = Mesh::POSITION;
                else if (data->attributeArrays[i].name == "normal")       attribute.semantic = Mesh::NORMAL;
                else if (data->attributeArrays[i].name == "tangent")      attribute.semantic = Mesh::TANGENT;
                else if (data->attributeArrays[i].name == "color")        attribute.semantic = Mesh::COLOR;
                else if (data->attributeArrays[i].name == "texcoord")     attribute.semantic = Mesh::TEXCOORD;
                else if (data->attributeArrays[i].name == "bone_index")   attribute.semantic = Mesh::BONE_INDEX;
                else if (data->attributeArrays[i].name == "bone_weight")  attribute.semantic = Mesh::BONE_WEIGHT;
                else attribute.semantic = Mesh::ATTRIBUTE;

                attributes.push_back(attribute);

                offset += data->attributeArrays[i].attribute_size();
            }
        }
    }

    dirtyVertexLayout();
}

Mesh::Mesh(const DESC& desc) :
    aabb(desc.aabb),
    indexType(desc.indexType),
    vertexBuffer(desc.vertexBuffer),
    indexBuffer(desc.indexBuffer),
    vertexSize(desc.vertexSize),
    attributes(desc.attributes, desc.attributes + desc.numAttributes)
{
    dirtyVertexLayout();
}

void Mesh::dirtyVertexLayout()
{
    // generate vertex layout
    std::vector<sgl::VertexLayout::ELEMENT> elements;

    bool ffp = (currentRenderer()->getRenderTechnique() == Renderer::FIXED_PIPELINE);
    for (size_t i = 0; i<attributes.size(); ++i)
    {
        sgl::VertexLayout::ELEMENT element;
        element.index    = attributes[i].binding->index;
        element.semantic = ffp ? BIND_SEMANTIC[attributes[i].semantic] : sgl::VertexLayout::ATTRIBUTE;
        element.offset   = attributes[i].offset;
        element.stride   = vertexSize;
        element.type     = attributes[i].type;
        element.size     = attributes[i].size;
        elements.push_back(element);
    }

    vertexLayout.reset( currentDevice()->CreateVertexLayout(elements.size(), &elements[0]) );
}

Mesh* Mesh::shallowCopy() const
{
    DESC desc;
    desc.aabb           = aabb;
    desc.numAttributes  = attributes.size();
    desc.indexType      = indexType;
    desc.attributes     = &attributes[0];
    desc.vertexBuffer   = vertexBuffer.get();
    desc.indexBuffer    = indexBuffer.get();
    desc.vertexSize     = vertexSize;

    Mesh* copy = new Mesh(desc);
	copy->data = data;
    for (size_t i = 0; i<subsets.size(); ++i)
    {
        if ( const plain_subset* subset = dynamic_cast<const plain_subset*>(subsets[i].get()) )
        {
            copy->subsets.push_back( new plain_subset(*subset) );
            copy->subsets.back()->mesh = copy;
        }
        else if ( const indexed_subset* subset = dynamic_cast<const indexed_subset*>(subsets[i].get()) )
        {
            copy->subsets.push_back( new indexed_subset(*subset) );
            copy->subsets.back()->mesh = copy;
        }
        else {
            assert(!"Invalid mesh subset");
        }
    }

    return copy;
}


} // namespace graphics
} // namespace slon
