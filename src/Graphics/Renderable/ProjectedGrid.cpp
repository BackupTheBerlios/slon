#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/ParameterBinding.h"
#include "Graphics/Renderable/ProjectedGrid.h"
#include "Scene/Visitors/CullVisitor.h"
#include <sgl/Math/Matrix.hpp>

using namespace std;
using namespace math;

using namespace slon;
using namespace scene;
using namespace graphics;

namespace {

    void setupQuadsGrid( int sizeX, 
                         int sizeY, 
                         std::vector<Vector2f>& vertices,
                         std::vector<unsigned>& indices )
    {
        // vertices
        vertices.resize(sizeX * sizeY);
        double dx = 1.0 / (sizeX - 1);
        double dy = 1.0 / (sizeY - 1);
        double y = 0.0;
        for(int i = 0; i<sizeY; ++i, y += dy)
        {
            double x = 0.0;
            for(int j = 0; j<sizeX; ++j, x += dx) {
                vertices[i*sizeX + j] = Vector2f( (float)x, (float)y  );
            }
        }

        // indices
        indices.resize( (sizeX - 1) * (sizeY - 1) * 4 );
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
    }

    void setupTriangleStripGrid( int sizeX, 
                                 int sizeY, 
                                 std::vector<Vector2f>& vertices,
                                 std::vector<unsigned>& indices )
    {
        // vertices
        vertices.resize(sizeX * sizeY + 2);
        vertices[0]                 = math::Vector2f(0.0f, 0.0f);
        vertices[sizeX * sizeY + 1] = (sizeY % 2 == 0) ? math::Vector2f(1.0f, 1.0f) : math::Vector2f(0.0f, 1.0f);

        double dx = 1.0 / (double(sizeX) - 0.5);
        double dy = 1.0 / (double(sizeY) - 1.0);
        double y  = 0.0;
        for(int i = 0; i<sizeY; ++i, y += dy)
        {
            double x = (i % 2 == 1) ? 0.0 : 0.5 * dx;
            for(int j = 0; j<sizeX; ++j, x += dx) {
                vertices[i*sizeX + j + 1] = Vector2f( (float)x, (float)y  );
            }
        }

        // indices
        indices.resize( (sizeY - 1) * sizeX * 2 + 2 );
        indices[0]                           = 0;
        indices[(sizeY - 1) * sizeX * 2 + 1] = sizeY * sizeX + 1;

        bool forward = true;
        for(int i = 0; i<sizeY-1; ++i)
        {
            if (forward)
            {
                for(int j = 0; j<sizeX; ++j)
                {
                    unsigned base     = (i*sizeX + j) * 2 + 1;
                    indices[base]     = (i+1)*sizeX + j + 1;
                    indices[base + 1] = i*sizeX + j + 1;
                }
            }
            else
            {
                for(int j = sizeX - 1; j>=0; --j)
                {
                    unsigned base     = (i*sizeX + (sizeX - j - 1)) * 2 + 1;
                    indices[base]     = (i+1)*sizeX + j + 1;
                    indices[base + 1] = i*sizeX + j + 1;
                }
            }
            forward = !forward;
        }
    }

    void setupTrianglesGrid( int sizeX, 
                             int sizeY, 
                             int stripLength,
                             std::vector<Vector2f>& vertices,
                             std::vector<unsigned>& indices )
    {
        // vertices
        vertices.resize(sizeX * sizeY);
        double dx = 1.0 / (double(sizeX) - 1.0);
        double dy = 1.0 / (double(sizeY) - 1.0);
        double y  = 0.0;
        for(int i = 0; i<sizeY; ++i, y += dy)
        {
            double x = 0.0;
            for(int j = 0; j<sizeX; ++j, x += dx) {
                vertices[i*sizeX + j] = Vector2f( (float)x, (float)y  );
            }
        }

        // indices
        indices.reserve( sizeY * (sizeX - 1) * 6 );
        for (int startX = 0; startX < sizeX - 1; startX += stripLength)
        {
            int maxJ = std::min(startX + stripLength, sizeX - 1);

            // warm strip
            for (int i = 0; i<stripLength; ++i) {
                indices.push_back(startX+i);
            }

            // fill row
            for(int i = 0; i<sizeY - 1; ++i)
            {
                for(int j = startX; j<maxJ; ++j)
                {
                    indices.push_back( (i+1)*sizeX + j );
                    indices.push_back( (i+1)*sizeX + j + 1 );
                    indices.push_back( i*sizeX + j );
                    indices.push_back( i*sizeX + j );
                    indices.push_back( (i+1)*sizeX + j + 1 );
                    indices.push_back( i*sizeX + j + 1 );
                }
            }
        }
    }

} // anonymous namespace 

ProjectedGrid::ProjectedGrid(const effect_ptr& _effect) :
    effect(_effect)
{
    assert(effect);
    setupGrid(128, 512);
    effect->bindParameter( hash_string("projectedGridCorners"), new parameter_binding<math::Vector3f>(corners, 4, true) );
    effect->bindParameter( hash_string("allowCulling"),  new parameter_binding<bool>(&allowCulling, 1, false) );
}

ProjectedGrid::ProjectedGrid(int sizeX, int sizeY, const effect_ptr& _effect) :
    effect(_effect)
{
    assert(effect);
    setupGrid(sizeX, sizeY);
    effect->bindParameter( hash_string("projectedGridCorners"), new parameter_binding<math::Vector3f>(corners, 4, true) );
    effect->bindParameter( hash_string("allowCulling"),  new parameter_binding<bool>(&allowCulling, 1, false) );
}

void ProjectedGrid::setupGrid(int sizeX, int sizeY)
{
    // vertices
    std::vector<Vector2f> vertices;
    std::vector<unsigned> indices;

    //setupQuadsGrid(sizeX, sizeY, vertices, indices);
    //primitiveType = sgl::QUADS;

    //setupTriangleStripGrid(sizeX, sizeY, vertices, indices);
    //primitiveType = sgl::TRIANGLE_STRIP;

    setupTrianglesGrid(sizeX, sizeY, 21, vertices, indices);
    primitiveType = sgl::TRIANGLES;
    allowCulling  = true;

    // setup data
    sgl::Device* device = currentDevice();
    {
        vertexBuffer.reset( device->CreateVertexBuffer() );
        vertexBuffer->SetData(vertices.size() * sizeof(math::Vector2f), &vertices[0]);

        indexBuffer.reset( device->CreateIndexBuffer() );
        indexBuffer->SetData(indices.size() * sizeof(unsigned int), &indices[0]);

        // retrieve binding
        positionBinding = detail::currentAttributeTable().queryAttribute( hash_string("position") );

        // make vertex layout
        sgl::VertexLayout::ELEMENT elements[] = 
        {
            {positionBinding->index, 2, 0, 8, sgl::FLOAT, sgl::VertexLayout::ATTRIBUTE}
        };
        vertexLayout.reset( device->CreateVertexLayout(1, elements) );
    }
}

void ProjectedGrid::accept(scene::CullVisitor& visitor) const
{
    if ( const Camera* camera = visitor.getCamera() )
    {
        math::Vector3f cameraPosition = math::get_translation( camera->getInverseViewMatrix() );
        if (camera && cameraPosition.y > 0.0f)
        {
            Matrix4f projMat   = camera->getProjectionMatrix();
            Matrix4f modelView = camera->getViewMatrix();
            Matrix4f clip      = projMat * modelView;

            // frustum clip planes
            Vector3f left   = Vector3f( clip(12) + clip(0),
                                        clip(13) + clip(1),
                                        clip(14) + clip(2) );

            Vector3f right  = Vector3f( clip(12) - clip(0),
                                        clip(13) - clip(1),
                                        clip(14) - clip(2) );

            Vector3f top    = Vector3f( clip(12) - clip(4),
                                        clip(13) - clip(5),
                                        clip(14) - clip(6) );

            Vector3f bottom = Vector3f( clip(12) + clip(4),
                                        clip(13) + clip(5),
                                        clip(14) + clip(6) );

            Vector4f farP = Vector4f( clip(12) - clip(8),
                                      clip(13) - clip(9),
                                      clip(14) - clip(10),
                                      clip(15) - clip(11) );
            farP /= length( xyz(farP) );

            // rays aligned with viewport
            corners[0] = normalize( cross(left, bottom) );
            corners[1] = normalize( cross(top, left) );
            corners[2] = normalize( cross(right, top) );
            corners[3] = normalize( cross(bottom, right) );

            // look up rays
            Vector3f lookUp[4];
            lookUp[0] = normalize( corners[3] - corners[0] );
            lookUp[1] = normalize( corners[0] - corners[1] );
            lookUp[2] = normalize( corners[3] - corners[2] );
            lookUp[3] = normalize( corners[1] - corners[0] );

            // intersect with plane
            for(int i = 0; i<4; ++i)
            {
                float L = -cameraPosition.y / corners[i].y;
                if ( corners[i].y >= 0.0f || L > farP.w )
                {
                    // intersect with far plane
                    L = -farP.w / dot( corners[i], math::Vector3f(farP.x, farP.y, farP.z) );
                    Vector3f frustumCorner = cameraPosition + L * corners[i];

                    // in analogy
                    L = -frustumCorner.y / lookUp[i].y;
                    corners[i] = frustumCorner + L * lookUp[i]; // intersection point
                }
                else
                {
                    L = -cameraPosition.y / corners[i].y;
                    corners[i] = cameraPosition + L * corners[i]; // intersection point
                }
            }

            // correct quad
            float L1 = length(corners[1] - corners[0]);
            float L2 = length(corners[2] - corners[3]);
            if ( L1 > L2 + 0.01f ) {
                corners[2] = corners[3] + (corners[2] - corners[3]) * L1 / L2;
            }
            else if ( L2 > L1 + 0.01f ) {
                corners[1] = corners[0] + (corners[1] - corners[0]) * L2 / L1;
            }

            // scale quad
            Vector3f center = (corners[0] + corners[1] + corners[2] + corners[3]) / 4.0f;
            for(int i = 0; i<4; ++i)
            {
                corners[i] += normalize(corners[i] - center) * 5.0f;
            }

            visitor.addRenderable(this);
        }
    }
}

void ProjectedGrid::render() const
{
    vertexBuffer->Bind( vertexLayout.get() );
    indexBuffer->Bind( sgl::IndexBuffer::UINT_32 );
    currentDevice()->DrawIndexed( primitiveType, 0, indexBuffer->Size() / sizeof(unsigned int) );
}
