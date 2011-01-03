#ifndef __SLON_ENGINE_GRAPHICS_GRAPHICS_MANAGER_H__
#define __SLON_ENGINE_GRAPHICS_GRAPHICS_MANAGER_H__

#include "Renderer.h"
#include <boost/signal.hpp>
#include <sgl/Device.h>
#include <vector>

#ifdef WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#elif defined(__linux__)
#   include <GL/glx.h>
#endif

namespace slon {

// forward
namespace scene 
{
    class Camera;
    typedef boost::intrusive_ptr<Camera> camera_ptr;
}

namespace graphics {

struct Surface
{
#ifdef WIN32
    HWND        hWnd;       // rendering window
    HDC			hDC;        // HDC of rendering window
    HGLRC       hRC;        // GL context for rendering into hDC
#elif defined(__linux__)
    Display*    display;
    GLXDrawable drawable;   // drawable for rendering
    GLXContext  context;    // GL context for rendering into target drawable
#endif

    Surface()
#ifdef WIN32
    :   hWnd(NULL)
    ,   hDC(NULL)
    ,   hRC(NULL)
#elif defined(__linux__)
    :   display(0)
    ,   drawable(0)
    ,   context(0)
#endif
    {
    }
};

/** The manager of the hole graphics.
 */
class GraphicsManager
{
public:
    typedef boost::signal<void ()>          pre_frame_render_signal;
    typedef boost::signal<void ()>          post_frame_render_signal;
    typedef boost::signals::connection      connection_type;

    typedef std::vector<scene::camera_ptr>  camera_vector;
    typedef camera_vector::iterator         camera_iterator;
    typedef camera_vector::const_iterator   camera_const_iterator;

public:
    /** Setup video mode
     * @param width - window width
     * @param height - window height
     * @param bpp - bits per pixel
     * @param fullscreen - enable fullscreen
     * @param vSync - enable vertical synchronization
     */
    virtual void setVideoMode( unsigned     width,
                               unsigned     height,
                               unsigned     bpp,
                               bool         fullscreen  = false,
                               bool         vSync       = false,
                               int          multisample = 0 ) = 0;

    /** Create forward renderer for rendering 3d scenes */
    virtual graphics::Renderer* initRenderer(const ForwardRendererDesc& desc) = 0;

    /** Create fixed pipeline renderer for rendering 3d scenes */
    virtual graphics::Renderer* initRenderer(const FFPRendererDesc& desc) = 0;

    /** Get renderer that renders 3d scene */
    virtual graphics::Renderer* getRenderer() = 0;

    /** Setup surface for rendering. This function will setup surface for rendering
     * and initialize graphics library if needed.
     * @param surface [in, out] - surface for rendering. Input parameters except window can be NULL,
     *  if so they will be created and returned through these variables. GraphicsManager will 
     *  not free window, context and other resources provided by this function.
     */
    virtual void setRenderingSurface(Surface& surface) = 0;

    /** Get rendering surface from current thread. Call this funtion, when you updated rendering
     * surface manually.
     */
    virtual const Surface& resolveRenderingSurface() = 0;

    /** Get current rendering surface. */
    virtual const Surface& getRenderingSurface() const = 0;

    /** Get sgl device for rendering */
    virtual sgl::Device* sglDevice() = 0;

    /** Attach pre frame render callback. It is called before frame rendering. */
    virtual connection_type connectFramePreRenderCallback(pre_frame_render_signal::slot_type slot) = 0;

    /** Attach pre frame render callback. It is called after frame rendering */
    virtual connection_type connectPostFrameRenderCallback(post_frame_render_signal::slot_type slot) = 0;

    /** Get first camera iterator */
    virtual camera_const_iterator firstCamera() const = 0;

    /** Get end camera iterator */
    virtual camera_const_iterator endCamera() const = 0;

    /** Add camera to the engine cameras vector. Cameras will
     * render scene consequently, in the order you have them added.
     * You should care about the uniqueness if the cameras.
     * Duplicate cameras will render scene twice, so take care
     * of it, if such behaviour is not your goal.
     */
    virtual void addCamera(scene::Camera* camera) = 0;

    /** Remove camera from the scene. Doesn't corrupt cameras order.
     * @param return true if camera removed.
     */
    virtual bool removeCamera(scene::Camera* camera) = 0;

    virtual ~GraphicsManager() {}
};

/** Get current graphics manager used by engine. */
GraphicsManager& currentGraphicsManager();

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_GRAPHICS_MANAGER_H__
