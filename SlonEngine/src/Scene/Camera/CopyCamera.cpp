#include "stdafx.h"
#include "Scene/Camera/CopyCamera.h"

namespace slon {
namespace scene {

CopyCamera::CopyCamera() 
{}

CopyCamera::CopyCamera(const Camera& camera)
{
    thread::lock_ptr lock = camera.lockForReading();
    viewMatrix          = camera.getViewMatrix();
    invViewMatrix       = camera.getInverseViewMatrix();
    normalMatrix        = camera.getNormalMatrix();
    frustum             = camera.getFrustum();
    projectionMatrix    = camera.getProjectionMatrix();
    bounds              = camera.getBounds();
    viewport            = camera.getViewport();
    postEffects         = camera.getPostEffectChain();
    renderTarget.reset( camera.getRenderTarget() );
}

} // namespace scene
} // namespace slon
