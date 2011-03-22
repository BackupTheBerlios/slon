#include "Graphics/Renderable/CameraAlignedQuad.h"
#include "Engine.h"

using namespace slon;
using namespace graphics;

CameraAlignedQuad::CameraAlignedQuad()
{
}

CameraAlignedQuad::CameraAlignedQuad(const boost::shared_ptr<Camera>& _masterCamera) :
    masterCamera(_masterCamera)
{
}

void CameraAlignedQuad::setupExponentialGrid( int     sizeX, 
                                              int     sizeY,
                                              double  power )
{
    using namespace math;

    // create mesh
    sgl::Mesh* surfaceMesh = new sgl::Mesh( Engine::Instance()->sglDevice() );

    // exponential attenuation
    double a = 1.0 / ( exp(power) - 1.0 );

    // vertices
    std::vector<Vector2f> vertices(sizeX * sizeY);
    double dx = 1.0 / (sizeX - 1);
    double dy = 1.0 / (sizeY - 1);
    double y  = 0.0; 
    for(int i = 0; i<sizeY; ++i)
    {
        double x = 0.0; 
        y = a * (exp( power*i*dy ) - 1.0);
        for(int j = 0; j<sizeX; ++j, x += dx)
        {
            vertices[i*sizeX + j] = Vector2f( (float)x, (float)y  );
        }
    }

    // indices
    std::vector<int> indices( (sizeX - 1) * (sizeY - 1) * 4 );
    for(int i = 0; i<sizeY - 1; ++i)
    {
        for(int j = 0; j<sizeX - 1; ++j)
        {
            int baseIndex = (i*(sizeX - 1) + j) * 4;
            indices[baseIndex]     = i * sizeX + j;
            indices[baseIndex + 1] = (i + 1) * sizeX + j;
            indices[baseIndex + 2] = (i + 1) * sizeX + j + 1;
            indices[baseIndex + 3] = i * sizeX + j + 1;
        }
    }

    // setup data
    surfaceMesh->SetVertices( &vertices[0], vertices.size() );
    surfaceMesh->SetVertexIndices( &indices[0], indices.size() );
    surfaceMesh->AddSubset( sgl::Mesh::subset( sgl::QUADS, 0, indices.size() ) );

    // setup data for water
    quadRenderable.removeSubsets();
    quadRenderable.setMesh(surfaceMesh);
    quadRenderable.addSubset(0);
}

// Override Geode
void CameraAlignedQuad::performOnRenderables(const function_on_renderable& func) 
{
    if ( quadRenderable.getMesh() ) {
        func(quadRenderable);
    }
}
