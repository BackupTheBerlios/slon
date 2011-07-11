#ifndef __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__
#define __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__

#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <sgl/RenderTarget.h>

namespace slon {
namespace graphics {

/** Abstract interface for every post process filter */
class PostProcessFilter :
    public Referenced
{
public:
    /** Perform compoumd post effect chain. 
     * @param renderTarget - render target for performing post effect. 
     * For many post effects render target must contain two attachments
     * at the attachment points 0 and 1 for performing ping-ponging.
     * @param source - ping pong source texture (0 or 1).
     * @return number of ping pong switches, usually 1.
     */
    virtual unsigned perform( sgl::RenderTarget*    renderTarget,
                              unsigned              source ) const = 0;

    virtual ~PostProcessFilter() {}
};

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__
