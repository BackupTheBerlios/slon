#ifndef __SLON_ENGINE_CAMER_SLAVE_CAMERA__
#define __SLON_ENGINE_CAMER_SLAVE_CAMERA__

#include "../Camera.h"

namespace slon {
namespace scene {

/** Can be used for creating moulds of the camera. */
class CopyCamera :
    public Camera
{
public:
    CopyCamera();

    /** Locks camera and copies content. */
    CopyCamera(const Camera& camera);

    // Override Entity
    const math::AABBf& getBounds() const    { return bounds; }

    // Override camera
    sgl::rectangle                  getViewport() const         { return viewport; }
    sgl::RenderTarget*              getRenderTarget() const     { return renderTarget.get(); }
    const graphics::filter_chain&   getPostEffectChain() const  { return postEffects; }

    const math::Frustumf& getFrustum() const            { return frustum; }
    const math::Matrix4f& getProjectionMatrix() const   { return projectionMatrix; }
    const math::Matrix4f& getViewMatrix() const         { return viewMatrix; }
    const math::Matrix4f& getInverseViewMatrix() const  { return invViewMatrix; }
    const math::Matrix3f& getNormalMatrix() const       { return normalMatrix; }

    // copy camera is thread safe
    thread::lock_ptr    lockForReading() const  { return thread::lock_ptr(); }
    thread::lock_ptr    lockForWriting()        { return thread::lock_ptr(); }

protected:
    math::Matrix4f  viewMatrix;
    math::Matrix4f  invViewMatrix;
    math::Matrix3f  normalMatrix;
    math::Frustumf  frustum;
    math::Matrix4f  projectionMatrix;

    math::AABBf     bounds;

    sgl::rectangle                  viewport;
    sgl::ref_ptr<sgl::RenderTarget> renderTarget;
    graphics::filter_chain          postEffects;
};

typedef boost::intrusive_ptr<CopyCamera>            copy_camera_ptr;
typedef boost::intrusive_ptr<const CopyCamera>      const_copy_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_CAMER_SLAVE_CAMERA__
