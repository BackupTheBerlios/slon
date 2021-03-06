#include "stdafx.h"
#include "Database/Archive.h"
#include "Database/Detail/SGLSerialization.h"
#include "Graphics/Common.h"
#include "Graphics/GPUSideMesh.h"
#include "Graphics/Renderer.h"
#include "Utility/error.hpp"
#include "Utility/math.hpp"

DECLARE_AUTO_LOGGER("graphics.GPUSideMesh")

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

GPUSideMesh::buffer_lock_impl::buffer_lock_impl(sgl::Buffer* buffer_,
                                         const int    mask) :
    buffer(buffer_)
{
    assert( buffer && !buffer->Mapped() );
    if ( sgl::SGL_OK != buffer->Map(mask, &ptr) ) {
        throw slon_error(AUTO_LOGGER, "Can't lock buffer");
    }
}

GPUSideMesh::buffer_lock_impl::~buffer_lock_impl()
{
    buffer->Unmap();
}

GPUSideMesh::plain_subset* GPUSideMesh::addPlainSubset( Effect*             effect,
                                          sgl::PRIMITIVE_TYPE primitiveType,
                                          unsigned            startVertex,
                                          unsigned            numVertices )
{
    plain_subset* subset = new plain_subset(this, effect, primitiveType, startVertex, numVertices);
    subsets.push_back( subset_ptr(subset) );
    return subset;
}

GPUSideMesh::indexed_subset* GPUSideMesh::addIndexedSubset( Effect*             effect,
                                              sgl::PRIMITIVE_TYPE primitiveType,
                                              unsigned            startIndex,
                                              unsigned            numIndices )
{
    indexed_subset* subset = new indexed_subset(this, effect, primitiveType, startIndex, numIndices);
    subsets.push_back( subset_ptr(subset) );
    return subset;
}

void GPUSideMesh::plain_subset::render() const
{
    mesh->vertexBuffer->Bind( mesh->vertexLayout.get() );
    currentDevice()->Draw(primitiveType, startVertex, numVertices);
}

void GPUSideMesh::indexed_subset::render() const
{
    mesh->vertexBuffer->Bind( mesh->vertexLayout.get() );
    mesh->indexBuffer->Bind( mesh->indexType );
    currentDevice()->DrawIndexed(primitiveType, startIndex, numIndices);
}

GPUSideMesh::GPUSideMesh()
{
}

GPUSideMesh::GPUSideMesh(const DESC& desc) :
    aabb(desc.aabb),
    indexType(desc.indexType),
    vertexBuffer(desc.vertexBuffer),
    indexBuffer(desc.indexBuffer),
    vertexSize(desc.vertexSize),
    attributes(desc.attributes, desc.attributes + desc.numAttributes)
{
    dirtyVertexLayout();
}

const char* GPUSideMesh::serialize(database::OArchive& ar) const
{
	database::serialize( ar, "aabb", aabb );
	ar.writeChunk( "indexType", reinterpret_cast<const int*>(&indexType) );
	ar.writeCustomSerializable( vertexBuffer.get() );
	ar.writeCustomSerializable( indexBuffer.get() );
    ar.writeChunk( "vertexSize", &vertexSize );
	
	ar.openChunk("subsets");
	{
		for (size_t i = 0; i<subsets.size(); ++i)
		{
			if ( indexed_subset* s = dynamic_cast<indexed_subset*>(subsets[i].get()) ) 
			{
				ar.openChunk( "indexed_subset" );
				ar.writeSerializable( s->effect.get() );
				ar.writeChunk( "primitiveType", reinterpret_cast<const int*>(&s->primitiveType) );
				ar.writeChunk( "startIndex", &s->startIndex );
				ar.writeChunk( "numIndices", &s->numIndices );
				ar.closeChunk();
			}
			else if ( plain_subset* s = dynamic_cast<plain_subset*>(subsets[i].get()) ) 
			{
				ar.openChunk( "plain_subset" );
				ar.writeSerializable( s->effect.get() );
				ar.writeChunk( "primitiveType", reinterpret_cast<const int*>(&s->primitiveType) );
				ar.writeChunk( "startVertex", &s->startVertex );
				ar.writeChunk( "numVertices", &s->numVertices );
				ar.closeChunk();
			}
			else {
				assert(false);
			}
		}
	}
	ar.closeChunk();

    ar.openChunk("attributes");
    {
		for (size_t i = 0; i<attributes.size(); ++i)
		{
            ar.openChunk("attribute");
            ar.writeStringChunk("name", attributes[i].binding->name.c_str(), attributes[i].binding->name.str().length() );
            ar.writeChunk("size", &attributes[i].size);
            ar.writeChunk("offset", &attributes[i].offset);
            ar.writeChunk("semantic", (int*)&attributes[i].semantic);
            ar.writeChunk("type", (int*)&attributes[i].type);
            ar.closeChunk();
        }
    }
    ar.closeChunk();

    return "GPUSideMesh";
}

void GPUSideMesh::deserialize(database::IArchive& ar)
{
	using namespace database;

    database::deserialize( ar, "aabb", aabb );
	ar.readChunk( "indexType", reinterpret_cast<int*>(&indexType) );
	vertexBuffer.reset( ar.readCustomSerializable<sgl::VertexBuffer>() );
	indexBuffer.reset( ar.readCustomSerializable<sgl::IndexBuffer>() );
	ar.readChunk( "vertexSize", &vertexSize );
	
    IArchive::chunk_info info;
	if ( ar.openChunk("subsets", info) )
	{
		while (true)
		{
			if ( ar.openChunk("indexed_subset", info) )
			{
				indexed_subset_ptr subset( new indexed_subset(this) );
				subset->effect = ar.readSerializable<Effect>();
				ar.readChunk("primitiveType", reinterpret_cast<int*>(&subset->primitiveType));
				ar.readChunk("startIndex", &subset->startIndex);
				ar.readChunk("numIndices", &subset->numIndices);
				ar.closeChunk();
			    subsets.push_back(subset);
			}
			else if ( ar.openChunk("plain_subset", info) ) 
			{
				plain_subset_ptr subset( new plain_subset(this) );
				subset->effect = ar.readSerializable<Effect>();
				ar.readChunk("primitiveType", reinterpret_cast<int*>(&subset->primitiveType));
				ar.readChunk("startVertex", &subset->startVertex);
				ar.readChunk("numVertices", &subset->numVertices);
				ar.closeChunk();
			    subsets.push_back(subset);
			}
			else {
				break;
			}
		}
	
		ar.closeChunk();
	}

	if ( ar.openChunk("attributes", info) )
    {
        std::string bindingName;
        attribute   attr;
		while ( ar.openChunk("attribute", info) )
		{
            ar.readStringChunk("name", bindingName);
            ar.readChunk("size", &attr.size);
            ar.readChunk("offset", &attr.offset);
            ar.readChunk("semantic", (int*)&attr.semantic);
            ar.readChunk("type", (int*)&attr.type);
            ar.closeChunk();

            attr.binding = detail::currentAttributeTable().queryAttribute(bindingName);
            attributes.push_back(attr);
        }

		ar.closeChunk();
    }

    dirtyVertexLayout();
}

void GPUSideMesh::dirtyVertexLayout()
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

gpu_side_mesh_ptr GPUSideMesh::clone(bool copyVBO, 
                                     bool copyIBO) const
{
    DESC desc;
    desc.aabb           = aabb;
    desc.numAttributes  = attributes.size();
    desc.indexType      = indexType;
    desc.attributes     = &attributes[0];
    desc.vertexBuffer   = vertexBuffer;
    if (copyIBO)
    {
        desc.indexBuffer.reset( currentDevice()->CreateIndexBuffer() );
        indexBuffer->CopyTo( desc.indexBuffer.get() );
    }
    else {
        desc.indexBuffer = indexBuffer;
    }
    if (copyVBO)
    {
        desc.vertexBuffer.reset( currentDevice()->CreateVertexBuffer() );
        vertexBuffer->CopyTo( desc.vertexBuffer.get() );
    }
    else {
        desc.vertexBuffer = vertexBuffer;
    }

    gpu_side_mesh_ptr copy(new GPUSideMesh(desc));
    for (size_t i = 0; i<subsets.size(); ++i)
    {
        if ( const plain_subset* subset = dynamic_cast<const plain_subset*>(subsets[i].get()) )
        {
            copy->subsets.push_back( new plain_subset(*subset) );
            copy->subsets.back()->mesh = copy.get();
        }
        else if ( const indexed_subset* subset = dynamic_cast<const indexed_subset*>(subsets[i].get()) )
        {
            copy->subsets.push_back( new indexed_subset(*subset) );
            copy->subsets.back()->mesh = copy.get();
        }
        else {
            assert(!"Invalid mesh subset");
        }
    }

    return copy;
}

} // namespace graphics
} // namespace slon
