#include "stdafx.h"
#include "Database/Archive.h"
#include "Physics/CollisionShape.h"

DECLARE_AUTO_LOGGER("physics.CollisionShape")

namespace slon {
namespace physics {

const char* PlaneShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("normal", plane.normal.arr, plane.normal.num_elements);
    ar.writeChunk("distance", &plane.distance);
    return "PlaneShape";
}

void PlaneShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("normal", plane.normal.arr, plane.normal.num_elements);
    ar.readChunk("distance", &plane.distance);
}

const char* SphereShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("radius", &radius);
    return "SphereShape";
}

void SphereShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("radius", &radius);
}

const char* BoxShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("halfExtent", halfExtent.arr, halfExtent.num_elements);
    return "BoxShape";
}

void BoxShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("halfExtent", halfExtent.arr, halfExtent.num_elements);
}

const char* ConeShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("height", &height);
    ar.writeChunk("radius", &radius);
    return "ConeShape";
}

void ConeShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("height", &height);
    ar.readChunk("radius", &radius);
}

const char* CapsuleShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("height", &height);
    ar.writeChunk("radius", &radius);
    return "CapsuleShape";
}

void CapsuleShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("height", &height);
    ar.readChunk("radius", &radius);
}

const char* CylinderShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk("halfExtent", halfExtent.arr, halfExtent.num_elements);
    return "CylinderShape";
}

void CylinderShape::deserialize(database::IArchive& ar)
{
    ar.readChunk("halfExtent", halfExtent.arr, halfExtent.num_elements);
}

const char* ConvexShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk( "vertices", vertices[0].arr, 3 * vertices.size() );
    return "ConvexShape";
}

void ConvexShape::deserialize(database::IArchive& ar)
{
    database::IArchive::chunk_info info;
    if ( !ar.openChunk("vertices", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Can't read vertices chunk");
    }
    else if (!info.isLeaf) {
        throw database::serialization_error(AUTO_LOGGER, "Vertices chunk is not leaf");
    }

    if (info.size > 0)
    {
        vertices.resize(info.size / 3);
        ar.read(&vertices[0].arr);
    }
    else {
        vertices.clear();
    }

    ar.closeChunk();
}

const char* TriangleMeshShape::serialize(database::OArchive& ar) const
{
    ar.writeChunk( "vertices", vertices[0].arr, 3 * vertices.size() );
    ar.writeChunk( "indices", &indices[0], indices.size() );
    return "TriangleMeshShape";
}

void TriangleMeshShape::deserialize(database::IArchive& ar)
{
    database::IArchive::chunk_info info;
    if ( !ar.openChunk("vertices", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Can't read vertices chunk");
    }
    else if (!info.isLeaf) {
        throw database::serialization_error(AUTO_LOGGER, "Vertices chunk is not leaf");
    }

    if (info.size > 0)
    {
        vertices.resize(info.size / 3);
        ar.read(&vertices[0].arr);
    }
    else {
        vertices.clear();
    }
	
    if ( !ar.openChunk("indices", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Can't read indices chunk");
    }
    else if (!info.isLeaf) {
        throw database::serialization_error(AUTO_LOGGER, "Indices chunk is not leaf");
    }

    if (info.size > 0)
    {
        indices.resize(info.size);
        ar.read(&indices[0]);
    }
    else {
        indices.clear();
    }

    ar.closeChunk();
}

const char* CompoundShape::serialize(database::OArchive& ar) const
{
    ar.openChunk("shapes");
    for (size_t i = 0; i<shapes.size(); ++i)
    {
        ar.openChunk("shape");
        ar.writeChunk("transform", shapes[i].transform.data(), shapes[i].transform.num_elements);
        ar.writeSerializable(shapes[i].shape.get());
        ar.closeChunk();
    }
    ar.closeChunk();

    return "CompoundShape";
}

void CompoundShape::deserialize(database::IArchive& ar)
{
    database::IArchive::chunk_info info;
    if ( !ar.openChunk("shapes", info) ) {
        throw database::serialization_error(AUTO_LOGGER, "Can't read shapes chunk");
    }

    while ( ar.openChunk("shape", info) )
    {
        shape_transform st;
        ar.readChunk("transform", st.transform.data(), st.transform.num_elements);
        st.shape = ar.readSerializable<CollisionShape>(true, true);
        shapes.push_back(st);
        ar.closeChunk();
    }

    ar.closeChunk();
}

} // namespace physics
} // namespace slon