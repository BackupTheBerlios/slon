#include "stdafx.h"
#include "Scene/Camera/RenderTargetCamera.h"
#include "Graphics/Common.h"

namespace slon {
namespace scene {

using namespace graphics;

RenderTargetCamera::RenderTargetCamera(sgl::RenderTarget* _renderTarget) :
    renderTarget(_renderTarget)
{
    if (renderTarget) {
        colorAttachments.resize(16);
    }
}
void RenderTargetCamera::setRenderTarget(sgl::RenderTarget* _renderTarget)
{
    renderTarget.reset(_renderTarget);
    if (_renderTarget)
    {
        for(size_t i = 0; i<colorAttachments.size(); ++i) {
            renderTarget->SetColorAttachment( i, colorAttachments[i].texture.get(), 0 );
        }
        renderTarget->SetDepthStencilAttachment( depthAttachment.texture.get(), 0 );

        colorAttachments.resize(16);
    }
}

sgl::Texture2D* RenderTargetCamera::getColorAttachment(unsigned int stage) const
{
    assert( stage < colorAttachments.size() );
    return colorAttachments[stage].texture.get();
}

sgl::Texture2D* RenderTargetCamera::getDepthAttachment() const
{
    return depthAttachment.texture.get();
}

void RenderTargetCamera::setViewport(const sgl::rectangle& _viewport)
{
    viewport = _viewport;
}

} // namespace scene
} // namespace slon
