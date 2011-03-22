#ifndef __SLON_ENGINE_SCENE_LIGHT_H__
#define __SLON_ENGINE_SCENE_LIGHT_H__

#include "Entity.h"

namespace slon {
namespace scene {

/** Interface for light sources.
 */
class Light :
    public scene::Entity
{
public:
    enum LIGHT_TYPE
    {
        AMBIENT,
        DIRECTIONAL,
        POINT,
        SPOT,
        VOLUME,
        NUM_LIGHT_TYPES
    };

public:
    /** Get light type */
    virtual LIGHT_TYPE getLightType() const = 0;

    /** Check if light casts shadow */
    virtual bool isShadowCaster() const = 0;

    virtual ~Light() {}
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_LIGHT_H__
