#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/PostProcess/Common.h"

using namespace slon;
using namespace graphics;
using namespace ppu;
using namespace sgl;

// static decl
namespace 
{
    sgl::ref_ptr<sgl::VertexBuffer>         screenQuadBuffer;
    sgl::ref_ptr<sgl::VertexLayout>         screenQuadLayout;
    sgl::ref_ptr<sgl::DepthStencilState>    depthStencilState;
    sgl::ref_ptr<sgl::RasterizerState>      rasterizerState;
    std::stack<sgl::rectangle>              viewportStack;
}

void DrawUtilities::beginChain()
{
    assert( viewportStack.size() < 1000 && "Too much viewports in stack!" );
    viewportStack.push( currentDevice()->Viewport() );

    sgl::Device* device = currentDevice();
    if (!screenQuadBuffer)
    {
        using namespace math;

        Vector2f vertices[4];
        vertices[0] = Vector2f(-0.000001f,  1.000001f);
        vertices[1] = Vector2f( 1.000001f,  1.000001f);
        vertices[2] = Vector2f( 1.000001f, -0.000001f);
        vertices[3] = Vector2f(-0.000001f, -0.000001f);

        sgl::VertexLayout::ELEMENT elements[] = 
        {
            {0, 2, 0, 8, sgl::FLOAT, sgl::VertexLayout::ATTRIBUTE},
        };
        screenQuadLayout.reset( device->CreateVertexLayout(1, elements) );

        screenQuadBuffer.reset( device->CreateVertexBuffer() );
        screenQuadBuffer->SetData(4 * sizeof(Vector2f), vertices);
    }
    screenQuadBuffer->Bind( screenQuadLayout.get() );

    if (!depthStencilState)
    {
        sgl::DepthStencilState::DESC desc;
        desc.depthEnable    = false;
        desc.depthWriteMask = 0;
        desc.stencilEnable  = false;
        depthStencilState.reset( device->CreateDepthStencilState(desc) );
    }

    if (!rasterizerState)
    {
        sgl::RasterizerState::DESC desc;
        desc.cullMode = sgl::RasterizerState::NONE;
        rasterizerState.reset( device->CreateRasterizerState(desc) );
    }

    device->PushState(sgl::State::DEPTH_STENCIL_STATE);
    device->PushState(sgl::State::RASTERIZER_STATE);
    depthStencilState->Bind();
    rasterizerState->Bind();
}

void DrawUtilities::endChain()
{
    assert( !viewportStack.empty() );

    sgl::Device* device = currentDevice();
    device->SetViewport( viewportStack.top() );
    viewportStack.pop();
    device->PopState(sgl::State::DEPTH_STENCIL_STATE);
    device->PopState(sgl::State::RASTERIZER_STATE);
}
