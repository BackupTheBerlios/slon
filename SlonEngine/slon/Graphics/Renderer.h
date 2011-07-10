#ifndef __SLON_ENGINE_GRAPHICS_RENDERER_H__
#define __SLON_ENGINE_GRAPHICS_RENDERER_H__

#include <boost/signals.hpp>
#include "../Realm/World.h"
#include "../Scene/Camera.h"
#include "../Scene/Light.h"
#include "../Scene/Visitor.h"
#include "Effect.h"

namespace slon {
namespace graphics {

struct FFPRendererDesc
{
    unsigned    bitsPerPixel;   /// 16, 24 or 32
    unsigned    depthBits;      /// 16 or 24
    unsigned    multisample;    /// 1 - no multisampling

    bool        useDebugRender; /// allow debug render

    FFPRendererDesc()
    :   bitsPerPixel(32)
    ,   depthBits(24)
    ,   multisample(1)
    ,   useDebugRender(false)
    {}
};

struct ForwardRendererDesc
{
    unsigned    bitsPerPixel;   /// 16, 24 or 32
    unsigned    depthBits;      /// 16 or 24
    unsigned    multisample;    /// 1 - no multisampling

    bool        makeDepthMap;   /// make depth map during depth pass or main pass
    bool        useDepthPass;   /// add depth only pass
    bool        useDebugRender; /// allow debug render

    ForwardRendererDesc()
    :   bitsPerPixel(32)
    ,   depthBits(24)
    ,   multisample(1)
    ,   makeDepthMap(false)
    ,   useDepthPass(false)
    ,   useDebugRender(false)
    {}
};

/* Renderer is an interface for classes performing different
 * rendering techniques, such as deffered shading or forward rendering.
 * Add scene wich you want to render as chid to this node.
 */
class Renderer :
    public Referenced
{
friend class scene::CullVisitor;
public:
    typedef boost::signal<void (const scene::Camera&)>  pre_render_signal;
    typedef boost::signal<void (const scene::Camera&)>  post_render_signal;
    typedef boost::signals::connection                  connection_type;

    enum RENDER_TECHNIQUE
    {
        FIXED_PIPELINE,
        FORWARD_RENDERING,
        DEFERRED_SHADING
    };

public:
    /** Get render technique used by the renderer. */
    virtual RENDER_TECHNIQUE getRenderTechnique() const = 0;

    /** Render scene to the device
     * @param sceneRoot - root node of the scene graph
     */
    virtual void render(realm::World& world, const scene::Camera& mainCamera) const = 0;

    /** Toggle wrireframe mode. */
    virtual void toggleWireframe(bool toggle) = 0;

    /** Get wireframe toggle. */
    virtual bool isWireframe() const = 0;

    /** Attach pre render callback. It is called just after buffer clearing. */
    virtual connection_type connectPreRenderCallback(pre_render_signal::slot_type slot) {
        return preRenderSignal.connect(slot);
    }

    /** Detach pre render callback. It is called just after buffer clearing. */
    virtual void disconnectPreRenderCallback(connection_type slot) {
        preRenderSignal.disconnect(slot);
    }

    /** Attach post render callback. It is called just before SwapBuffers. */
    virtual connection_type connectPostRenderCallback(post_render_signal::slot_type slot) {
        return postRenderSignal.connect(slot);
    }

    /** Detach post render callback. It is called just before SwapBuffers. */
    virtual void disconnectPostRenderCallback(connection_type slot) {
        postRenderSignal.disconnect(slot);
    }

protected:
    // callbacks
    pre_render_signal   preRenderSignal;
    post_render_signal  postRenderSignal;
};

typedef boost::intrusive_ptr<Renderer>          renderer_ptr;
typedef boost::intrusive_ptr<const Renderer>    const_renderer_ptr;

/** Get current renderer used by engine. */
Renderer* currentRenderer();

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERER_H__
