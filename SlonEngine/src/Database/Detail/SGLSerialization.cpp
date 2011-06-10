#include "stdafx.h"
#include "Database/Detail/SGLSerialization.h"

void SerializableWrapper<sgl::VertexLayout>::serialize(OArchive& ar) const
{
	ar.openChunk("elements");
	for (size_t i = 0; i<obj->NumElements(); ++i)
	{
		sgl::VertexLayout::Element elem = obj->Element(i);
		ar.openChunk("element");
		ar.writeChunk("index", &elem.index);
		ar.writeChunk("size", &elem.size);
		ar.writeChunk("offset", &elem.offset);
		ar.writeChunk("stride", &elem.stride);
		ar.writeChunk("type", &elem.type);
		ar.writeChunk("semantic", &elem.semantic);
		ar.closeChunk();
	}
	ar.closeChunk();
}

void SerializableWrapper<sgl::VertexLayout>::deserialize(IArchive& ar)
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
			ar.readChunk("type", &elem.type);
			ar.readChunk("semantic", &elem.semantic);
			ar.closeChunk();
		}
		elements.push_back(elem);
		ar.closeChunk();
	}

	obj = graphics::currentDevice()->CreateVertexLayout(elements.size(), &elements[0]);
}

void SerializableWrapper<sgl::VertexBuffer>::serialize(OArchive& ar) const
{
	char* data;
	obj->Map(STATIC_READ, &data);
	ar.writeChunk( "data", data, obj->Size() );
	obj->Unmap();
}

void SerializableWrapper<sgl::VertexLayout>::deserialize(IArchive& ar)
{
	obj = graphics::currentDevice()->CreateVertexBuffer();

	IArchive::info info;
	if ( ar.openChunk("data", info) )
	{

	}
}