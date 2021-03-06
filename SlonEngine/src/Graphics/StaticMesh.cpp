#include "stdafx.h"
#include "Database/Archive.h"
#include "Graphics/StaticMesh.h"
#include "Scene/CullVisitor.h"
#include "Scene/TransformVisitor.h"
#include "Utility/math.hpp"

namespace slon {
namespace graphics {

StaticMesh::StaticMesh()
{
}

StaticMesh::StaticMesh(const gpu_side_mesh_ptr& _mesh) :
	mesh(_mesh),
	shadowCaster(true),
	shadowReceiver(true)
{
    assert(mesh);

    // bind parameters
    for (GPUSideMesh::subset_iterator iter  = mesh->firstSubset();
                               iter != mesh->endSubset();
                               ++iter)
    {
        (*iter)->getEffect()->bindParameter( hash_string("worldMatrix"), new parameter_binding<math::Matrix4f>(&worldMatrix, 1, true) );
    }
}
    
StaticMesh::~StaticMesh()
{
}

const char* StaticMesh::serialize(database::OArchive& ar) const
{
	Node::serialize(ar);
	ar.writeSerializable(mesh.get());
    return "StaticMesh";
}

void StaticMesh::deserialize(database::IArchive& ar)
{
	Node::deserialize(ar);
	mesh = static_cast<GPUSideMesh*>( ar.readSerializable() );

    // bind parameters
    for (GPUSideMesh::subset_iterator iter  = mesh->firstSubset();
                               iter != mesh->endSubset();
                               ++iter)
    {
        (*iter)->getEffect()->bindParameter( hash_string("worldMatrix"), new parameter_binding<math::Matrix4f>(&worldMatrix, 1, true) );
    }
}

// Override node
void StaticMesh::accept(scene::CullVisitor& visitor) const
{
    for( GPUSideMesh::subset_const_iterator subsetIter  = mesh->firstSubset();
                                     subsetIter != mesh->endSubset();
                                     ++subsetIter )
    {
        visitor.addRenderable( subsetIter->get() );
    }
}

void StaticMesh::accept(scene::TransformVisitor& visitor)
{
    worldMatrix = visitor.getLocalToWorldTransform();
}

} // namespace graphics
} // namespace slon