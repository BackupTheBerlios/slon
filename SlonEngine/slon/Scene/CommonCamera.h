#ifndef __SLON_ENGINE_SCENE_COMMON_CAMERA__
#define __SLON_ENGINE_SCENE_COMMON_CAMERA__

#include "../Utility/math.hpp"
#include "../Graphics/PostProcessFilter.h"
#include "Camera.h"

#define NOMINMAX // thread may include windows.h
#include <boost/thread/shared_mutex.hpp>

namespace slon {
namespace scene {

/** Common camera handles common functions for common cameras.
 * Derived cameras must check lock mutex at any state modification functions.
 * Common camera creates AmbientPass by default.
 */
class SLON_PUBLIC CommonCamera :
    public Camera
{
public:
    CommonCamera();

    /** Set render target of the camera */
    virtual void setRenderTarget(sgl::RenderTarget* _renderTarget);

    /** Set viewport of the camera */
    virtual void setViewport(const sgl::rectangle& _viewport);

    /** Bounds of the camera specifies update area. */
    virtual const math::AABBf& getBounds() const;

    /** Get mutable post effect chain. */
    virtual graphics::post_process_filter_chain& getPostEffectChain()        { return postEffects; }

    // Override Camera
    sgl::RenderTarget*                           getRenderTarget() const     { return renderTarget.get(); }
    sgl::rectangle                               getViewport() const         { return viewport; }
    const graphics::post_process_filter_chain&   getPostEffectChain() const  { return postEffects; }

    thread::lock_ptr    lockForReading() const;
    thread::lock_ptr    lockForWriting();

    virtual ~CommonCamera() {}

protected:
    sgl::ref_ptr<sgl::RenderTarget>     renderTarget;
    graphics::post_process_filter_chain postEffects;
    sgl::rectangle                      viewport;

    // update area
    mutable math::AABBf             updateArea;

    // mutex locks any camera modification
    mutable boost::shared_mutex     accessMutex;
};

typedef boost::intrusive_ptr<CommonCamera>            common_camera_ptr;
typedef boost::intrusive_ptr<const CommonCamera>      const_common_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_COMMON_CAMERA__
