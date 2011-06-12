#include "stdafx.h"
#include "Database/Archive.h"
#include "Database/Detail/SGLSerialization.h"

DECLARE_AUTO_LOGGER("database.SGLSerialization")

namespace slon {
namespace database {

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

void SerializableWrapper<sgl::VertexLayout>::deserialize(const sgl::VertexLayout*& vl, IArchive& ar)
{
	std::vector<sgl::VertexLayout::Element> elements;
	if ( ar.openChunk("elements") )
	{
		sgl::VertexLayout::Element elem;
		while ( ar.openChunk("element") )
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

const char* SerializableWrapper<sgl::Buffer>::serialize(const sgl::VertexBuffer& buffer, OArchive& ar)
{
	char* data = 0;
	obj->Map(STATIC_READ, &data);
	ar.writeChunk( "data", data, obj->Size() );
	obj->Unmap()
}

void SerializableWrapper<sgl::Buffer>::deserialize(sgl::Buffer*& buffer, IArchive& ar)
{
    assert(buffer);
	IArchive::chunk_info info;
	if ( ar.openChunk("data", info) )
	{
        char* data = (char*)malloc(info.size);
        if (!data) {
            throw serialization_error(AUTO_LOGGER, "Can't allocate data for buffer");
        }

        ar.read(data);
        buffer->SetData(info.size, data);
        free(data);
	}
}

const char* SerializableWrapper<sgl::VertexBuffer>::serialize(const sgl::VertexBuffer& buffer, OArchive& ar)
{
    SerializableWrapper<sgl::Buffer>::serialize(buffer, ar);
    return "VertexBuffer";
}

void SerializableWrapper<sgl::VertexBuffer>::deserialize(sgl::VertexBuffer*& buffer, IArchive& ar)
{
	buffer = graphics::currentDevice()->CreateVertexBuffer();
    SerializableWrapper<sgl::Buffer>::deserialize(buffer, ar);
}

} // namespace database
} // namespace slon