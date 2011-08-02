#include "stdafx.h"
#include "Database/Archive.h"
#include "Database/Detail/SGLSerialization.h"
#include "Graphics/Common.h"

DECLARE_AUTO_LOGGER("database.SGLSerialization")

namespace slon {
namespace database {

template<>
const char* SerializableWrapper<sgl::VertexLayout>::serialize(const sgl::VertexLayout& vl, OArchive& ar)
{
	ar.openChunk("elements");
	for (size_t i = 0; i<vl.NumElements(); ++i)
	{
		sgl::VertexLayout::ELEMENT elem = vl.Element(i);
		ar.openChunk("element");
		ar.writeChunk("index", &elem.index);
		ar.writeChunk("size", &elem.size);
		ar.writeChunk("offset", &elem.offset);
		ar.writeChunk("stride", &elem.stride);
		ar.writeChunk("type", reinterpret_cast<const int*>(&elem.type));
		ar.writeChunk("semantic", reinterpret_cast<const int*>(&elem.semantic));
		ar.closeChunk();
	}
	ar.closeChunk();

    return "VertexLayout";
}

template<>
void SerializableWrapper<sgl::VertexLayout>::deserialize(sgl::VertexLayout*& vl, IArchive& ar)
{
	std::vector<sgl::VertexLayout::ELEMENT> elements;

    IArchive::chunk_info info;
	if ( ar.openChunk("elements", info) )
	{
		sgl::VertexLayout::ELEMENT elem;
		while ( ar.openChunk("element", info) )
		{
			ar.readChunk("index", &elem.index);
			ar.readChunk("size", &elem.size);
			ar.readChunk("offset", &elem.offset);
			ar.readChunk("stride", &elem.stride);
			ar.readChunk("type", reinterpret_cast<int*>(&elem.type));
			ar.readChunk("semantic", reinterpret_cast<int*>(&elem.semantic));
			ar.closeChunk();
		}
		elements.push_back(elem);
		ar.closeChunk();
	}

	vl = graphics::currentDevice()->CreateVertexLayout(elements.size(), &elements[0]);
}

template<>
const char* SerializableWrapper<sgl::Buffer>::serialize(const sgl::Buffer& buffer, OArchive& ar)
{
	void* data = malloc( buffer.Size() );
    if (!data) {
        throw serialization_error(AUTO_LOGGER, "Can't allocate memory for buffer data.");
    }

    if ( sgl::SGL_OK != buffer.GetData( data, 0, buffer.Size() ) )
    {
        free(data);
        throw serialization_error(AUTO_LOGGER, "Can't read data from buffer.");
    }

	ar.writeBinaryChunk( "data", data, buffer.Size() );
    free(data);

    return "Buffer";
}

template<>
void SerializableWrapper<sgl::Buffer>::deserialize(sgl::Buffer*& buffer, IArchive& ar)
{
    assert(buffer);
	IArchive::chunk_info info;
	if ( ar.openChunk("data", info) )
	{
        void* data = malloc(info.size);
        if (!data) {
            throw serialization_error(AUTO_LOGGER, "Can't allocate memory for buffer data.");
        }

        ar.read(data);
        buffer->SetData(info.size, data);
        free(data);
	}
}

template<>
const char* SerializableWrapper<sgl::VertexBuffer>::serialize(const sgl::VertexBuffer& buffer, OArchive& ar)
{
    SerializableWrapper<sgl::Buffer>::serialize(buffer, ar);
    return "VertexBuffer";
}

template<>
void SerializableWrapper<sgl::VertexBuffer>::deserialize(sgl::VertexBuffer*& buffer, IArchive& ar)
{
	buffer = graphics::currentDevice()->CreateVertexBuffer();
    sgl::Buffer* pBuf = buffer;
    SerializableWrapper<sgl::Buffer>::deserialize(pBuf, ar);
}

template<>
const char* SerializableWrapper<sgl::IndexBuffer>::serialize(const sgl::IndexBuffer& buffer, OArchive& ar)
{
    SerializableWrapper<sgl::Buffer>::serialize(buffer, ar);
    return "IndexBuffer";
}

template<>
void SerializableWrapper<sgl::IndexBuffer>::deserialize(sgl::IndexBuffer*& buffer, IArchive& ar)
{
	buffer = graphics::currentDevice()->CreateIndexBuffer();
    sgl::Buffer* pBuf = buffer;
    SerializableWrapper<sgl::Buffer>::deserialize(pBuf, ar);
}

} // namespace database
} // namespace slon
