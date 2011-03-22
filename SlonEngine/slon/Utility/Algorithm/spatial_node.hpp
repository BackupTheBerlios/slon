#ifndef __SLON_ENGINE_REFERENCED_H__
#define __SLON_ENGINE_REFERENCED_H__

#include "../../Config.h"
#include <sgl/Utility/Aligned.h>

namespace slon {

/** Base class for all spatial node structures */
struct spatial_node
#ifdef SLON_ENGINE_USE_SSE
    : public sgl::Aligned16
#endif
{
private:
    // noncopyable
    spatial_node(const spatial_node&);
    spatial_node& operator = (const spatial_node&);

public:
    spatial_node() {}

protected:
    ~spatial_node() {}
};

} // namespace slon

#endif // __SLON_ENGINE_REFERENCED_H__
