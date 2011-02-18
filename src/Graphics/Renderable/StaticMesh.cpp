#include "stdafx.h"
#include "Graphics/Renderable/StaticMesh.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Utility/math.hpp"

using namespace slon;
using namespace scene;
using namespace graphics;

StaticMesh::StaticMesh(Mesh* _mesh) :
	mesh(_mesh),
	shadowCaster(true),
	shadowReceiver(true)
{
    assert(mesh);

    // bind parameters
    for (Mesh::subset_iterator iter  = mesh->firstSubset();
                               iter != mesh->endSubset();
                               ++iter)
    {
        (*iter)->getEffect()->bindParameter( unique_string("worldMatrix"), new parameter_binding<math::Matrix4f>(&worldMatrix, 1, true) );
    }
}

// Override node
void StaticMesh::accept(scene::CullVisitor& visitor) const
{
    for( Mesh::subset_const_iterator subsetIter  = mesh->firstSubset();
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
