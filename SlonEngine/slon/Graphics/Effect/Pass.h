#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_PASS_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_PASS_H__

#include <boost/intrusive_ptr.hpp>
#include "../../Utility/referenced.hpp"

namespace slon {
namespace graphics {

/** Setup shader and states necessary for rendering object. */
class Pass :
    public Referenced
{
public:
    /** Get priority of the effect pass. This used to sort objects in the render queue.
     * @return priority in the render queue. Long long because
     * it is common to use pointers as priorities.
     */
    virtual long long getPriority() const = 0;

    /** Setup pass states. */
    virtual void begin() const = 0;

    /** End using technique. Restore states. */
    virtual void end() const = 0;

    virtual ~Pass() {}
};

typedef boost::intrusive_ptr<Pass>         pass_ptr;
typedef boost::intrusive_ptr<const Pass>   const_pass_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_PASS_H__
