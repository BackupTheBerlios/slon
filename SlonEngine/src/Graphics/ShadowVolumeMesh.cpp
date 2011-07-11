#include "Graphics/Common.h"
#include "Graphics/Renderable/ShadowVolumeMesh.h"
#include <map>

using namespace slon;
using namespace graphics;
using namespace math;

ShadowVolumeMesh::mesh_map ShadowVolumeMesh::svMeshes;

ShadowVolumeMesh::ShadowVolumeMesh(graphics::Mesh* mesh)
{
    effect.reset( new ExtrudeEffect() );
    setupMesh(mesh);
}

/** Create shadow volume mesh from the sgl mesh
 * @param mesh - sgl mesh for shadow volume
 */
void ShadowVolumeMesh::setupMesh(graphics::Mesh* mesh)
{
	// generate edge map
	typedef std::pair<int, int>		edge;
	typedef std::map<edge, edge>	edge_map;

	std::vector<Vector3f>	svVertices( mesh->NumVertexIndices() );
	std::vector<Vector3f>	svNormals( mesh->NumVertexIndices() );
	std::vector<int>		svIndices( mesh->NumVertexIndices() * 4 );
	std::vector<edge>		adjacentEdges( mesh->NumVertexIndices() );

	// copy data
	const Vector3f*	vertices = mesh->QueryVertexData<Vector3f>();
	const int*		indices = mesh->VertexIndices();

    // mesh doesn't have vertices or indices
    if (!vertices || !indices)
    {
        svMesh.reset();
        return;
    }

	// process data
	for(size_t i = 0; i<mesh->NumVertexIndices(); i += 3)
	{
		Vector3f normal = normalize( cross( vertices[ indices[i+1] ] - vertices[ indices[i] ],
											vertices[ indices[i+2] ] - vertices[ indices[i] ] ) );

		for(size_t j = 0; j<3; ++j)
		{
			svVertices[i+j] = vertices[ indices[i+j] ];
			svIndices[i+j] = i + j;
			svNormals[i+j] = normal;
		}
	}

	// create edge_map
	edge_map edgeMap;
	for(size_t i = 0; i<mesh->NumVertexIndices(); i += 3)
	{
		edgeMap.insert( edge_map::value_type( edge(indices[i], indices[i+1]), edge(i, i+1) ) );
		edgeMap.insert( edge_map::value_type( edge(indices[i+1], indices[i+2]), edge(i+1, i+2) ) );
		edgeMap.insert( edge_map::value_type( edge(indices[i+2], indices[i]), edge(i+2, i) ) );
	}

	// find adjacent faces
	for(size_t i = 0; i<mesh->NumVertexIndices(); i += 3)
	{
		for(size_t j = 0; j<3; ++j)
		{
			int a = indices[i + j];
			int b = indices[i + (j+1)%3];
			edge_map::const_iterator adjacentFaceIter = edgeMap.find( edge(b, a) );
			if ( adjacentFaceIter == edgeMap.end() )
			{
				// mesh contains holes
			    svMesh.reset();
			    return;
			}
			adjacentEdges[i+j] = adjacentFaceIter->second;
		}
	}

	// adjacent
	size_t stride = mesh->NumVertexIndices();
	for(size_t i = 0; i<mesh->NumVertexIndices(); i += 3)//mesh->GetNumVertexIndices()
	{
		svIndices[stride + i]  = i;
		svIndices[stride + i + 1] = adjacentEdges[i].second;
		svIndices[stride + i + 2] = i + 1;

		svIndices[2*stride + i]  = i + 1;
		svIndices[2*stride + i + 1] = adjacentEdges[i + 1].second;
		svIndices[2*stride + i + 2] = i + 2;

		svIndices[3*stride + i]  = i + 2;
		svIndices[3*stride + i + 1] = adjacentEdges[i + 2].second;
		svIndices[3*stride + i + 2] = i;
	}

	// set data
    MeshFactory meshFactory;
    {
        svMesh.setAttributes( sgl::VertexLayout::VERTEX, &svVertices[0], svVertices.size() );
	    svMesh.setAttributes( sgl::VertexLayout::NORMAL, &svNormals[0], svNormals.size() );
	    svMesh.setAttrbitueIndices( sgl::VertexLayout::VERTEX, &svIndices[0], svIndices.size() );
	    svMesh.setAttrbitueIndices( sgl::VertexLayout::NORMAL, &svIndices[0], svIndices.size() );
    }
    svMesh.reset( meshFactory.createMesh() );
    svMesh->addIndexedSubset( effect.get(), sgl::TRIANGLES, 0, svIndices.size() );

	// add new shadow volume mesh to storage
	svMeshes.insert( mesh_map::value_type(mesh, svMesh.get()) );
}

/** Render mesh to the device */
void ShadowVolumeMesh::render() const
{
    assert(svMesh);

    svMesh->Bind();
    svMesh->DrawSubset(0);
    svMesh->Unbind();
}
