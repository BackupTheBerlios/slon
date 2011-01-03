#ifndef SLON_ENGINE_DEFERRED_RENDERER_H
#define SLON_ENGINE_DEFERRED_RENDERER_H

#include "Graphics/Renderer.h"

namespace slon {

namespace graphics {

/* DeferredRenderer implements deffered shading rendering technique
 */
class DeferredRenderer :
    public Renderer
{
public:
    // Override Renderer
    RENDER_TECHNIQUE getRenderTechnique() const { return DEFERRED_SHADING; }

    virtual ~DeferredRenderer() {}
};

} // namespace graphics

} // namespace slon

#endif // SLON_ENGINE_DEFERRED_RENDERER_H
