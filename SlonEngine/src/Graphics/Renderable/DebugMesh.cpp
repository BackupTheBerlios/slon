#include "stdafx.h"
#include "Graphics/Detail/AttributeTable.h"
#include "Graphics/Renderable/DebugMesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Common.h"
#include "Scene/Visitor/CullVisitor.h"
#include "Scene/Visitor/TransformVisitor.h"
#include "Utility/math.hpp"

namespace slon {
namespace graphics {

DebugMesh::DebugMesh() :
    totalNumVertices(0),
    stateChanged(true),
    infiniteBounds(false),
    useCameraProjection(true),
	baseTransform( math::make_identity<float, 4>() ),
    transform( math::make_identity<float, 4>() ),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    textSize(10, 12),
    geometryDirty(false),
    depthTest(true),
    wireframe(false)
{
    // create vertex layout
    sgl::Device* device = currentDevice();
    if ( currentRenderer()->getRenderTechnique() == Renderer::FIXED_PIPELINE )
    {
        sgl::VertexLayout::ELEMENT elements[] = 
        {
            {0, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::VERTEX}
        };
        vertexLayout.reset( device->CreateVertexLayout(1, elements) );
    }
    else
    {
        detail::AttributeTable::binding_ptr positionBinding = detail::currentAttributeTable().queryAttribute( hash_string("position") );

        sgl::VertexLayout::ELEMENT elements[] = 
        {
            {positionBinding->index, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::ATTRIBUTE}
        };
        vertexLayout.reset( device->CreateVertexLayout(1, elements) );
    }

    aabb.reset_max();
}

void DebugMesh::accept(scene::TransformVisitor& visitor)
{
    baseTransform = visitor.getLocalToWorldTransform();
}

void DebugMesh::accept(scene::CullVisitor& visitor) const
{
    for (size_t i = 0; i<subsets.size(); ++i) {
        visitor.addRenderable(&subsets[i]); 
    }

    for (size_t i = 0; i<textSubsets.size(); ++i) {
        visitor.addRenderable(&textSubsets[i]); 
    }
}

void DebugMesh::dirty()
{
    // calculate bounds
    if (infiniteBounds) {
        aabb.reset_max();
    } 
    else
    {
        aabb.reset_min();
        for (vertices_vector::const_iterator iter  = vertices.begin();
                                             iter != vertices.end();
                                             ++iter)
        {
            aabb.extend(*iter);
        }
    }

    if (!vertexBuffer) {
        vertexBuffer.reset( currentDevice()->CreateVertexBuffer() );
    }

    if (!indexBuffer) {
        indexBuffer.reset( currentDevice()->CreateIndexBuffer() );
    }

    size_t size = vertices.size() * sizeof(math::Vector3f);
    if (vertexBuffer->Size() >= size) {
        vertexBuffer->SetSubData(0, size, &vertices[0]);
    }
    else {
        vertexBuffer->SetData(size, &vertices[0], sgl::Buffer::STREAM_DRAW);
    }

    size = indices.size() * sizeof(unsigned int);
    if (indexBuffer->Size() >= size) {
        indexBuffer->SetSubData(0, size, &indices[0]);
    }
    else {
        indexBuffer->SetData(size, &indices[0], sgl::Buffer::STREAM_DRAW);
    }

    geometryDirty = false;
}

void DebugMesh::clear(bool clearBuffers) 
{
    aabb.reset_max();
    totalNumVertices = 0;
    geometryDirty    = false;
    vertices.clear();
    textSubsets.clear();
    subsets.clear();
    indices.clear();
    
    if (clearBuffers)
    {
        vertexBuffer.reset();
        indexBuffer.reset();
    }
}

void DebugMesh::pushPrimitive( sgl::PRIMITIVE_TYPE primType,
                               size_t              numVertices )
{
    if ( stateChanged 
         || subsets.empty()
         || primType != subsets.back().primitiveType
         || primType == sgl::LINE_STRIP 
         || primType == sgl::LINE_LOOP 
         || primType == sgl::TRIANGLE_STRIP 
         || primType == sgl::TRIANGLE_FAN
         || primType == sgl::QUAD_STRIP )
    {
        // add new subset
        subset s;
        {
            DebugEffect::DESC desc;
            desc.color               = color;
            desc.depthTest           = depthTest;
            desc.wireframe           = wireframe;
            desc.modelMatrix         = transform;
            desc.projectionMatrix    = projection;
            desc.useCameraProjection = useCameraProjection;
			
			if ( !subsets.empty() ) 
			{
				// this is just optimization
				s.debugEffect.reset( new DebugEffect(*subsets.back().debugEffect) );
				s.debugEffect->reset(desc);
			}
			else {
				s.debugEffect.reset( new DebugEffect(desc) );
			}
        }
        s.debugEffect->bindParameter( hash_string("worldMatrix"), new parameter_binding<math::Matrix4f>(&baseTransform, 1) );
        s.debugMesh     = this;
        s.transform     = transform;
        s.primitiveType = primType;
        s.vertexOffset  = totalNumVertices;
        s.numVertices   = numVertices;
        subsets.push_back(s);

        stateChanged     = false;
    }
    else
    {
        // extend subset
        subsets.back().numVertices += numVertices;
    }

    if (!useCameraProjection) 
    {
        // can't determine bounds if not in camera space
        infiniteBounds = true;
    }

    totalNumVertices += numVertices;
}


void DebugMesh::pushTextPrimitive(const std::string& text)
{
    infiniteBounds = true;

    text_subset s;
    s.text      = text;
    s.position  = math::xy( math::get_translation(transform) );
    s.debugEffect.reset( new DebugTextEffect(font.get(), textSize, color) );
    textSubsets.push_back(s);
}

const math::AABBf& DebugMesh::getBounds() const
{
    if (geometryDirty) {
        const_cast<DebugMesh*>(this)->dirty();
    }

    return aabb;
}

void DebugMesh::text_subset::render() const
{
    debugEffect->getFont()->Print( position.x, position.y, text.c_str() );
}

void DebugMesh::subset::render() const
{
    if (debugMesh->geometryDirty) {
        const_cast<DebugMesh*>(debugMesh)->dirty();
    }

    debugMesh->vertexBuffer->Bind( debugMesh->vertexLayout.get() );
    debugMesh->indexBuffer->Bind(sgl::IndexBuffer::UINT_32);
    currentDevice()->DrawIndexed(primitiveType, vertexOffset, numVertices);
}

} // namesapce graphics
} // namespace slon
