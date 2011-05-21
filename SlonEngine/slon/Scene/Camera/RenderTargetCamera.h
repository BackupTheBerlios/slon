#ifndef __SLON_ENGINE_CAMERA_RENDER_TARGET_CAMERA__
#define __SLON_ENGINE_CAMERA_RENDER_TARGET_CAMERA__

#include <sgl/RenderTarget.h>
#include "../../Scene/Camera.h"

namespace slon {
namespace scene {

class RenderTargetCamera :
    public Camera
{
private:
    struct attachment
    {
        attachment() :
            autoSize(false)
        {}

        attachment(sgl::Texture2D* _texture, bool _autoSize) :
            texture(_texture),
            autoSize(_autoSize)
        {}

        sgl::ref_ptr<sgl::Texture2D> texture;
        bool autoSize;
    };

    typedef std::vector<attachment> attachment_vector;

public:
    RenderTargetCamera(sgl::RenderTarget* renderTarget = 0);

    /** Get color attachment used by camera
     * @stage - mrt index of the color attachment in the render target
     */
    virtual sgl::Texture2D* getColorAttachment(unsigned int stage) const;

    /** Get depth attachment used by camera */
    virtual sgl::Texture2D* getDepthAttachment() const;

    /** Get render target of the camera */
    virtual sgl::RenderTarget* getRenderTarget() const { return renderTarget.get(); }

    /** Set render target of the camera */
    virtual void setRenderTarget(sgl::RenderTarget* _renderTarget);

    /** Set viewport of the camera */
    virtual void setViewport(const sgl::rectangle& _viewport);

    /** Get viewport of the camera */
    virtual sgl::rectangle getViewport() const { return viewport; }

    virtual ~RenderTargetCamera() {}

protected:
    sgl::rectangle viewport;

    // Make render target for the child camera
    // virtual void createRenderTarget() = 0;

private:
    sgl::ref_ptr<sgl::RenderTarget> renderTarget;
    attachment_vector               colorAttachments;
    attachment                      depthAttachment;
};

typedef boost::intrusive_ptr<RenderTargetCamera>            render_target_camera_ptr;
typedef boost::intrusive_ptr<const RenderTargetCamera>      const_render_target_camera_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_CAMERA_RENDER_TARGET_CAMERA__
