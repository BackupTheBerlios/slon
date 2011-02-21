#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__

#include "../../Scene/Light.h"
#include "../Material/LightingMaterial.h"
#include "TransformEffect.h"

namespace slon {
namespace graphics {

/** Effect that generalizes different lighting techniques */
class LightingEffect :
    public TransformEffect
{
public:
    // maximum number of supported lights
    static const int MIN_NUM_LIGHTS = 1;
    static const int MAX_NUM_LIGHTS = 3;

public:
    typedef TransformEffect base_type;

public:
    LightingEffect(const LightingMaterial*          material,
                   LightingMaterial::dirty_signal&  dirtySignal);

    // Override Effect
    int                               present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    const abstract_parameter_binding* getParameter(hash_string name) const;
    bool                              bindParameter(hash_string                        name,
                                                    const abstract_parameter_binding*    binding);
    int                               queryAttribute(hash_string attribute);

private:
    void        dirty(const LightingMaterial* material);

private:
    // parameters
    math::Matrix4f          worldViewMatrix;
    math::Matrix3f          normalMatrix;
    float                   opacity;

    // bindings
    binding_int_ptr         lightCountBinder;
    binding_float_ptr       opacityOneBinder;
    const_binding_float_ptr opacityBinder;

    // effect connection
    const_lighting_material_ptr         material;
    boost::signals::scoped_connection   dirtyConnection;

    // technique
    pass_ptr                ffpPass;
    pass_ptr                passes[scene::Light::NUM_LIGHT_TYPES][MAX_NUM_LIGHTS];
    pass_ptr                backFacePasses[scene::Light::NUM_LIGHT_TYPES][MAX_NUM_LIGHTS];
};

typedef boost::intrusive_ptr<LightingEffect>        lighting_effect_ptr;
typedef boost::intrusive_ptr<const LightingEffect>  const_lighting_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__
