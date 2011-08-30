#ifndef __SLON_ENGINE_GRAPHICS_DETAIL_GRAPHICS_MANAGER_H__
#define __SLON_ENGINE_GRAPHICS_DETAIL_GRAPHICS_MANAGER_H__

#include "../Common.h"
#include "../GraphicsManager.h"
#include "../Renderer.h"
#include "AttributeTable.h"
#include "UniformTable.h"

namespace slon {
namespace graphics {
namespace detail {

/** The manager of the hole graphics.
 */
class GraphicsManager :
    public graphics::GraphicsManager
{
public:
    GraphicsManager();

    // Override GraphicsManager
    void setVideoMode( unsigned     width,
                       unsigned     height,
                       unsigned     bpp,
                       bool         fullscreen = false,
                       bool         vSync = false,
                       int          multisample = 0 );

    graphics::Renderer* initRenderer(const ForwardRendererDesc& desc);
    graphics::Renderer* initRenderer(const FFPRendererDesc& desc);

    void            setRenderingSurface(Surface& surface);
    const Surface&  getCurrentRenderingSurface();
    const Surface&  getRenderingSurface() const;

    graphics::Renderer* getRenderer()   { return renderer.get(); }
    sgl::Device*        sglDevice()     { return device.get(); }

    connection_type connectFramePreRenderCallback(pre_frame_render_signal::slot_type slot) {
        return preFrameRenderSignal.connect(slot);
    }

    connection_type connectPostFrameRenderCallback(post_frame_render_signal::slot_type slot) {
        return postFrameRenderSignal.connect(slot);
    }

    camera_const_iterator firstCamera() const   { return cameras.begin(); }
    camera_const_iterator endCamera() const     { return cameras.end(); }

    void addCamera(scene::Camera* camera) { cameras.push_back(camera); }
    bool removeCamera(scene::Camera* camera);

    /** Get attribute table used by all used effects. */
    detail::AttributeTable& getAttributeTable() { return *attributeTable; }

    /** Get parameter table used by effects. */
    detail::ParameterTable& getParameterTable() { return *parameterTable; }

    /** Get uniform table used by effects. */
    detail::UniformTable& getUniformTable() { return *uniformTable; }

    // Called by Engine
    void render(realm::World& world);

private:
    // sgl
    sgl::ref_ptr<sgl::Device>   device;
    Surface                     surface;

    // video settings
    unsigned                    bitsPerPixel;
    unsigned                    depthBits;
    unsigned                    stencilBits;
    unsigned                    multisample;

    // internal managers, order is important!
    detail::attribute_table_ptr attributeTable;
    detail::parameter_table_ptr parameterTable;
    detail::uniform_table_ptr   uniformTable;
    renderer_ptr                renderer;
    camera_vector               cameras;

    // signals
    pre_frame_render_signal     preFrameRenderSignal;
    post_frame_render_signal    postFrameRenderSignal;
};

} // namepsace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_DETAIL_GRAPHICS_MANAGER_H__
