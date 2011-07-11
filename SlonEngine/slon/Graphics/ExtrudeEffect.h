#ifndef SLON_ENGINE_GRAPHICS_EFFECT_EXTRUDE_EFFECT_H
#define SLON_ENGINE_GRAPHICS_EFFECT_EXTRUDE_EFFECT_H

#include "../../Scene/Light.h"
#include "TransformEffect.h"

namespace slon {
namespace graphics {

// forward decl
class ShadowVolumePass;

/** Effect extrudes silhouette mesh of the shadow volume */
class ExtrudeEffect :
    public TransformEffect
{
public:
    ExtrudeEffect();

    // Override Effect
    Pass* present(const RenderPass& pass);

private:
    void dirtyShaderTechniques();

public:
    // shared among effects
    pass_ptr   passes[scene::Light::NUM_LIGHT_TYPES];
};

} // namespace graphics
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_EFFECT_EXTRUDE_EFFECT_H
