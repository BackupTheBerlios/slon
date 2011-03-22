#include "stdafx.h"
#include "Graphics/Renderable/Debug/DebugDrawCommon.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace slon {
namespace graphics {
namespace debug {

DebugMesh& operator << (DebugMesh& mesh, const debug::font& f)
{
    mesh.font = f.textFont;
    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::text_size& ts)
{
    mesh.textSize = ts.size;
    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::text& t)
{
    mesh.pushTextPrimitive(t.str);
    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::color& c)
{
    mesh.color        = c.colorVal;
    mesh.stateChanged = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::projection& p)
{
    mesh.useCameraProjection = p.useCameraProjection;
    mesh.projection          = p.matrix;
    mesh.stateChanged        = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::transform& t)
{
    mesh.transform    = t.matrix;
    mesh.stateChanged = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::depth_test& d)
{
    mesh.depthTest    = d.toggle;
    mesh.stateChanged = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const wireframe& w)
{
    mesh.wireframe      = w.toggle;
    mesh.stateChanged   = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const DebugMesh& other)
{
    size_t numVertices = mesh.vertices.size();
    std::copy( other.vertices.begin(), other.vertices.end(), std::back_inserter(mesh.vertices) );
    std::transform( other.indices.begin(), other.indices.end(), std::back_inserter(mesh.indices), std::bind2nd(std::plus<size_t>(), numVertices) );

    math::Matrix4f                  transform = mesh.transform;
    math::Vector4f                  color     = mesh.color;
    math::Vector2i                  textSize  = mesh.textSize;
    bool                            depthTest = mesh.depthTest;
    sgl::ref_ptr<const sgl::Font>   font      = mesh.font;
    {
        for (size_t i = 0; i<other.subsets.size(); ++i) 
        {
            mesh.transform = transform * other.subsets[i].transform;
            //mesh.color     = other.subsets[i].debugEffect->getColor();
            //mesh.depthTest = other.subsets[i].debugEffect->getDepthTestToggle();
            mesh.pushPrimitive(other.subsets[i].primitiveType, other.subsets[i].numVertices);
        }

        for (size_t i = 0; i<other.textSubsets.size(); ++i) 
        {
            mesh.transform = transform * math::make_translation(other.textSubsets[i].position.x, other.textSubsets[i].position.y, 0.0f);
            //mesh.color     = other.textSubsets[i].debugEffect->getColor();
            //mesh.textSize  = other.textSubsets[i].debugEffect->getSize();
            //mesh.font.reset( other.textSubsets[i].debugEffect->getFont() );
            mesh.pushTextPrimitive(other.textSubsets[i].text);
        }
    }
    mesh.transform     = transform;
    mesh.color         = color;
    mesh.textSize      = textSize;
    mesh.depthTest     = depthTest;
    mesh.font          = font;
    mesh.geometryDirty = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const debug::line& l)
{
    mesh.indices.push_back( mesh.vertices.size() );
    mesh.indices.push_back( mesh.vertices.size() + 1 );
    mesh.vertices.push_back(l.a);
    mesh.vertices.push_back(l.b);
    mesh.pushPrimitive(sgl::LINES, 2);
    mesh.geometryDirty = true;
    
    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const sector& s)
{
    assert(s.fullCircleSplits > 0);

    // tip
    size_t base = mesh.vertices.size();
    mesh.vertices.push_back( math::Vector3f(0.0f, 0.0f, 0.0f) );
    mesh.indices.push_back(base);
    
    // arc
	unsigned splits = unsigned(s.fullCircleSplits * fabs(fmodf(s.hiLimit - s.loLimit, math::PI2)) / math::PI2);
	if (splits == 0) {
		splits = 1;
	}

    for (unsigned i = 0; i < splits + 1; ++i)
    {
        float          angle   = s.loLimit + i * (s.hiLimit - s.loLimit) / splits;
        math::Vector3f pointer = math::make_rotation(angle, s.cross) * s.up;
        mesh.vertices.push_back(pointer);
        mesh.indices.push_back(base + i + 1);
    }
    
    mesh.wireframe = !s.filled;
    if (s.filled) {
        mesh.pushPrimitive(sgl::TRIANGLE_FAN, splits + 2);
    }
    else {
        mesh.pushPrimitive(sgl::LINE_LOOP, splits + 2);
    }
    mesh.geometryDirty = true;

    return mesh;
}

DebugMesh& operator << (DebugMesh& mesh, const cone& c)
{    
    // make tip
    DebugMesh coneMesh;
    coneMesh.vertices.resize(c.numVertices + 1);

    // make cap
    coneMesh.vertices[0] = math::Vector3f(0.0f, c.height * 0.5f, 0.0f);
    for (size_t i = 1; i<c.numVertices + 1; ++i)
    {
        float angle = math::PI2 * float(i) / c.numVertices;
        float x     = c.radius * cos(angle);
        float z     = c.radius * sin(angle);

        coneMesh.vertices[i] = math::Vector3f(x, -c.height * 0.5f, z);
    }

    if (c.wired)
    {
        coneMesh.indices.resize(c.numVertices * 6 + 6);
        for (size_t i = 1; i<c.numVertices + 1; ++i)
        {
            coneMesh.indices[i*6]     = 0;
            coneMesh.indices[i*6 + 1] = i;
            coneMesh.indices[i*6 + 2] = i;
            coneMesh.indices[i*6 + 3] = i+1;
            coneMesh.indices[i*6 + 4] = i+1;
            coneMesh.indices[i*6 + 5] = 0;
        }
        coneMesh.indices[c.numVertices * 6 + 3] = 1;
        coneMesh.indices[c.numVertices * 6 + 4] = 1;

        // make subset
        coneMesh.pushPrimitive( sgl::LINES, coneMesh.indices.size() );
    }
    else
    {
        coneMesh.indices.resize(c.numVertices * 2 + 1);

        // make cone
        for (size_t i = 0; i<c.numVertices + 1; ++i) {
            coneMesh.indices[i] = i;
        }
        coneMesh.pushPrimitive( sgl::TRIANGLE_FAN, c.numVertices + 1 );

        // make base
        for (size_t i = 1; i<c.numVertices + 1; ++i) {
            coneMesh.indices[i + c.numVertices] = i;
        }
        coneMesh.pushPrimitive( sgl::TRIANGLE_FAN, c.numVertices );
    }

    // dirty
    return mesh << coneMesh;
}

DebugMesh& operator << (DebugMesh& mesh, const cylinder& c)
{
    // make tip
    DebugMesh cylMesh;
    cylMesh.vertices.resize(c.numVertices * 2);

    // make cap
    for (size_t i = 0; i<c.numVertices; ++i)
    {
        float angle = math::PI2 * float(i) / (c.numVertices - 1);
        float x     = c.radius * cos(angle);
        float z     = c.radius * sin(angle);

        cylMesh.vertices[i] = math::Vector3f(x, -c.height * 0.5f, z);
        cylMesh.vertices[i + c.numVertices] = math::Vector3f(x, c.height * 0.5f, z);
    }

    if (c.wired)
    {
        cylMesh.indices.reserve(c.numVertices * 4);

        // make bottom cap
        for (size_t i = 0; i<c.numVertices; ++i) {
            cylMesh.indices.push_back(i);
        }
        cylMesh.pushPrimitive(sgl::LINE_LOOP, c.numVertices);

        // make top cap
        for (size_t i = 0; i<c.numVertices; ++i) {
            cylMesh.indices.push_back(i + c.numVertices);
        }
        cylMesh.pushPrimitive(sgl::LINE_LOOP, c.numVertices);

        // make walls
        for (size_t i = 0; i<c.numVertices; ++i) 
        {
            cylMesh.indices.push_back(i);
            cylMesh.indices.push_back(i + c.numVertices);
        }
        cylMesh.pushPrimitive(sgl::LINES, c.numVertices * 2);
    }
    else
    {
        cylMesh.indices.reserve(c.numVertices * 4);

        // make bottom cap
        for (size_t i = 0; i<c.numVertices; ++i) {
            cylMesh.indices.push_back(i);
        }
        cylMesh.pushPrimitive(sgl::TRIANGLE_FAN, c.numVertices);

        // make top cap
        for (size_t i = 0; i<c.numVertices; ++i) {
            cylMesh.indices.push_back(i + c.numVertices);
        }
        cylMesh.pushPrimitive(sgl::TRIANGLE_FAN, c.numVertices);

        // make walls
        for (size_t i = 0; i<c.numVertices; ++i) 
        {
            cylMesh.indices.push_back(i);
            cylMesh.indices.push_back(i + c.numVertices);
        }
        cylMesh.pushPrimitive(sgl::TRIANGLE_STRIP, c.numVertices * 2);
    }

    // dirty
    return mesh << cylMesh;
}

DebugMesh& operator << (DebugMesh& mesh, const math::AABBf& a)
{
    using namespace math;

    DebugMesh boxMesh;

    // Create vertices for box
    boxMesh.vertices.push_back( Vector3f(a.minVec.x, a.minVec.y, a.maxVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.minVec.x, a.maxVec.y, a.maxVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.maxVec.x, a.maxVec.y, a.maxVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.maxVec.x, a.minVec.y, a.maxVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.minVec.x, a.minVec.y, a.minVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.minVec.x, a.maxVec.y, a.minVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.maxVec.x, a.maxVec.y, a.minVec.z) );
    boxMesh.vertices.push_back( Vector3f(a.maxVec.x, a.minVec.y, a.minVec.z) );

    // Back face
    boxMesh.indices.push_back(0);
    boxMesh.indices.push_back(1);
    boxMesh.indices.push_back(2);
    boxMesh.indices.push_back(3);

    // Top face
    boxMesh.indices.push_back(1);
    boxMesh.indices.push_back(5);
    boxMesh.indices.push_back(6);
    boxMesh.indices.push_back(2);

    // Front face
    boxMesh.indices.push_back(5);
    boxMesh.indices.push_back(4);
    boxMesh.indices.push_back(7);
    boxMesh.indices.push_back(6);

    // Bottom face
    boxMesh.indices.push_back(4);
    boxMesh.indices.push_back(0);
    boxMesh.indices.push_back(3);
    boxMesh.indices.push_back(7);

    // make subset
    boxMesh.pushPrimitive(sgl::LINE_STRIP, 4);
    boxMesh.pushPrimitive(sgl::LINE_STRIP, 4);
    boxMesh.pushPrimitive(sgl::LINE_STRIP, 4);
    boxMesh.pushPrimitive(sgl::LINE_STRIP, 4);

    return mesh << boxMesh;
}

} // namespace debug
} // namespace graphics
} // namespace slon
