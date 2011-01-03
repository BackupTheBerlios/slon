#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Renderable/SkyBox.h"
#include "Graphics/Renderer.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Log/Logger.h"

using namespace slon;
using namespace graphics;

__DEFINE_LOGGER__("graphics.SkyBox")

SkyBox::SkyBox()
{
    using namespace math;

    const float sqrt3f = sqrtf(3.0f);

    // Create vertices for box
	std::vector<Vector3f> vertices;
    vertices.push_back( Vector3f(-sqrt3f, -sqrt3f,  sqrt3f) );
    vertices.push_back( Vector3f(-sqrt3f,  sqrt3f,  sqrt3f) );
    vertices.push_back( Vector3f( sqrt3f,  sqrt3f,  sqrt3f) );
    vertices.push_back( Vector3f( sqrt3f, -sqrt3f,  sqrt3f) );
    vertices.push_back( Vector3f(-sqrt3f, -sqrt3f, -sqrt3f) );
    vertices.push_back( Vector3f(-sqrt3f,  sqrt3f, -sqrt3f) );
    vertices.push_back( Vector3f( sqrt3f,  sqrt3f, -sqrt3f) );
    vertices.push_back( Vector3f( sqrt3f, -sqrt3f, -sqrt3f) );

    // Create an index array for the box
	std::vector<unsigned short> indices;

    // Front face
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    // Back face
    indices.push_back(4);
    indices.push_back(7);
    indices.push_back(6);
    indices.push_back(5);

    // Right face
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(3);
    indices.push_back(2);

    // Left face
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(1);

    // Top face
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(2);

    // Bottom face
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(4);

	// create mesh
    sgl::Device* device = currentDevice();
    {
        if ( currentRenderer()->getRenderTechnique() == Renderer::FIXED_PIPELINE )
        {
            sgl::VertexLayout::ELEMENT elements[] = 
            {
                {0, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::VERTEX},
                {0, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::TEXCOORD}
            };
            vertexLayout.reset( device->CreateVertexLayout(2, elements) );
        }
        else
        {
            positionBinding = detail::currentAttributeTable().queryAttribute( unique_string("position") );

            sgl::VertexLayout::ELEMENT elements[] = 
            {
                {positionBinding->index, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::ATTRIBUTE}
            };
            vertexLayout.reset( device->CreateVertexLayout(1, elements) );
        }

        vbo.reset( device->CreateVertexBuffer() );
        vbo->SetData( vertices.size() * sizeof(math::Vector3f), &vertices[0] );

        ibo.reset( device->CreateIndexBuffer() );
        ibo->SetData( indices.size() * sizeof(unsigned short), &indices[0] );
    }

    // create effect
    effect.reset( new SkyBoxEffect() );
}

void SkyBox::MakeFromSideTextures(const std::string maps[6])
{
    using namespace sgl;

    Device* device = currentDevice();
    {
        sgl::TextureCube::DESC      desc;
        sgl::ref_ptr<sgl::Image>    sideImages[6];
	    for(int i = 0; i<6; ++i)
        {
            sideImages[i].reset( device->CreateImage() );
            if ( SGL_OK != sideImages[i]->LoadFromFile( maps[i].c_str() ) )
            {
                logger << log::WL_WARNING << "Unable to load image form file: " + maps[i] << std::endl;
            }

            desc.sides[i].format = sideImages[i]->Format();
            desc.sides[i].width  = sideImages[i]->Width();
            desc.sides[i].height = sideImages[i]->Height();
            desc.sides[i].data   = sideImages[i]->Data(0);
        }

        cubemap.reset( device->CreateTextureCube(desc) );
        if (!cubemap) 
        {
            logger << log::WL_WARNING << "Unable to create cubemap image\n";
            return;
        }
        
        bool haveMipmaps = false;
        if ( sgl::SGL_OK != cubemap->GenerateMipmap() ) {
            logger << log::WL_WARNING << "Unable to generate cubemap mipmaps\n";
        }
        else {
            haveMipmaps = true;
        }

        // setup mip data if presented
        for (int i = 0; i<6; ++i)
        {
            if (sideImages[i]->NumMipmaps() > 1) 
            {
                sgl::Texture2D* side   = cubemap->Side( sgl::TextureCube::SIDE(i) );
                unsigned        width  = std::max<unsigned>( side->Width()  >> 1, 1);
                unsigned        height = std::max<unsigned>( side->Height() >> 1, 1);

                for (unsigned j = 1; j<sideImages[i]->NumMipmaps(); ++j) 
                {
                    side->SetSubImage( j, 0, 0, width, height, sideImages[i]->Data(j) );
                    width  = std::max<unsigned>( side->Width()  >> 1, 1);
                    height = std::max<unsigned>( side->Height() >> 1, 1);
                }

                haveMipmaps = true;
            }
        }

        sgl::SamplerState::DESC ssDesc;
        ssDesc.filter[0]  = sgl::SamplerState::LINEAR;
        ssDesc.filter[1]  = sgl::SamplerState::LINEAR;
        ssDesc.filter[2]  = haveMipmaps ? sgl::SamplerState::LINEAR : sgl::SamplerState::NONE;
        
        ssDesc.wrapping[0] = sgl::SamplerState::CLAMP_TO_EDGE;
        ssDesc.wrapping[1] = sgl::SamplerState::CLAMP_TO_EDGE;
        ssDesc.wrapping[2] = sgl::SamplerState::CLAMP_TO_EDGE;

        sgl::SamplerState* samplerState = device->CreateSamplerState(ssDesc);
        cubemap->BindSamplerState(samplerState);
    }

    effect->setEnvironmentMap( cubemap.get() );
}

void SkyBox::render() const
{
	// Render
	vbo->Bind( vertexLayout.get() );
    ibo->Bind( sgl::IndexBuffer::UINT_16 );
    currentDevice()->DrawIndexed(sgl::QUADS, 0, 24);
    vbo->Unbind();
    ibo->Unbind();
}

void SkyBox::accept(scene::CullVisitor& visitor)
{
    visitor.addRenderable(this);
}
