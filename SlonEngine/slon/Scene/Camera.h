#ifndef __SLON_ENGINE_SCENE_CAMERA_H__
#define __SLON_ENGINE_SCENE_CAMERA_H__

#include "../Graphics/Filter.h"
#include "../Thread/Lock.h"
#include "Entity.h"
#include <sgl/RenderTarget.h>
#include <sgl/Math/Frustum.hpp>

namespace slon {

// forward
namespace graphics {
    class GraphicsManager;
}

namespace scene {

/** Abstract base class for camera classes */
class Camera :
    public scene::Entity
{
public:
    // Override Node
    TYPE getNodeType() const { return CAMERA; }

    /** Get frustum */
    virtual const math::Frustumf& getFrustum() const = 0;

    /** Get projection transformation matrix */
    virtual const math::Matrix4f& getProjectionMatrix() const = 0;

    /** Get world to view space transformation matrix */
    virtual const math::Matrix4f& getViewMatrix() const = 0;

    /** Get view to world space transformation matrix */
    virtual const math::Matrix4f& getInverseViewMatrix() const = 0;

    /** Get inverse transpose transformation matrix. */
    virtual const math::Matrix3f& getNormalMatrix() const = 0;

    /** Get viewport of the camera */
    virtual sgl::rectangle getViewport() const = 0;

    /** Get render pass associated with the camera. This will
     * be added to the frame during the initial cull traverse.
     */
    virtual sgl::RenderTarget* getRenderTarget() const = 0;

    /** Get posteffect chain */
    virtual const graphics::filter_chain& getPostEffectChain() const = 0;

    /** Grant thread read access to the camera.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForReading() const = 0;

    /** Grant thread write access to the camera.
     * @return lock object. Lock is freed whether object is deleted.
     */
    virtual thread::lock_ptr lockForWriting() = 0;

    virtual ~Camera() {}
};

typedef boost::intrusive_ptr<Camera>            camera_ptr;
typedef boost::intrusive_ptr<const Camera>      const_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_CAMERA_H__
