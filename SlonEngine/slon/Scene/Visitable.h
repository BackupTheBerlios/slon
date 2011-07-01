#ifndef __SLON_ENGINE_SCENE_VISITABLE_H__
#define __SLON_ENGINE_SCENE_VISITABLE_H__

#include "Forward.h"

namespace slon {
namespace scene {

/** Base class for nodes which accept visitors. */
class Visitable
{
public:
    typedef long long visitor_mask;

public:
    

    virtual ~Visitable() {}
};

} // namespace scene
} // namespace slon

#endif __SLON_ENGINE_SCENE_VISITABLE_H__