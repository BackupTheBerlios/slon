#ifndef __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__
#define __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__

#include <boost/intrusive_ptr.hpp>
#include <list>
#include <sgl/RenderTarget.h>
#include "../Utility/referenced.hpp"

namespace slon {
namespace graphics {

/** Abstract interface for every post process filter */
class Filter :
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
    virtual ~Filter() {}
};

typedef boost::intrusive_ptr<Filter>        filter_ptr;
typedef boost::intrusive_ptr<const Filter>  const_filter_ptr;
typedef std::list<const_filter_ptr>         filter_chain;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_POST_PROCESS_FILTER_H__
