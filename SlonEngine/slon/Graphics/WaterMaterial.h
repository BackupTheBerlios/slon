#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__

#include <boost/signal.hpp>
#include <sgl/Texture2D.h>
#include "../../Scene/Light.h"
#include "../Effect.h"

namespace slon {
namespace graphics {

/** Effect that generalizes different lighting techniques */
class LightingEffect :
    public Effect
{
public:
    typedef boost::signal<void (const LightingEffect*)>     dirty_signal;

public:
    LightingEffect();

    /** Setup emission material color for the effect */
    void setEmission(const math::Vector4f& _emission)               { emission = _emission; }

    /** Setup diffuse material color for the effect */
    void setDiffuseSpecular(const math::Vector4f& _diffuseSpecular) { diffuseSpecular = _diffuseSpecular; }

    /** Setup material shininess for the effect */
    void setShininess(float _shininess)                             { shininess = _shininess; }

    /** Setup map containing emission color and power in alpha. */
    void setEmissionMap(sgl::Texture2D* _emissionMap);

    /** Setup map containing diffuse color in the rgb components and specular power in alpha. 
     * If diffuse map doesn't contain alpha component specular will be disabled.
     */
    void setDiffuseSpecularMap(sgl::Texture2D* _diffuseSpecularMap);

    /** Setup map containing normal in the rgb components and height in alpha. 
     * If map contain alpha component parallax occlusion technique will be used.
     */
    void setNormalHeightMap(sgl::Texture2D* _normalHeightMap);

    /** Setup opacity value for the effect
     * @param _opacity - measure of the object transparency.
     *   0.0 - fully transparent, 1.0 - not transparent
     */
    void setOpacity(float _opacity);

    /** Get emission color */
    const math::Vector4f& getEmission() const           { return emission; }

    /** Get diffuse material color */
    const math::Vector4f& getDiffuseSpecular() const    { return diffuseSpecular; }

    /** Get material shininess*/
    float getShininess() const                          { return shininess; }

    /** Get emission map of the effect */
    sgl::Texture2D* getEmissionMap() const          { return emissionMap.get(); }

    /** Get diffuse map of the effect */
    sgl::Texture2D* getDiffuseSpecularMap() const   { return diffuseSpecularMap.get(); }

    /** Get normal height map of the effect */
    sgl::Texture2D* getNormalHeightMap() const      { return normalHeightMap.get(); }

    /** Get opacity value of the effect */
    float getOpacity() const { return opacity; }

    // Override Effect
    Technique*  present(const RenderPass& pass)                                         { assert(!"Use Proxy for LightingEffect"); return 0; }
    bool        bindParameter(const std::string& name, const parameter_binder& binder)  { assert(!"Use Proxy for LightingEffect"); return false; }
    int         queryAttribute(const std::string& name)                                 { assert(!"Use Proxy for LightingEffect"); return -1; }
    Effect*     createProxy() const;

private:
    // properties
    math::Vector4f					emission;
    math::Vector4f					diffuseSpecular;
    float                           opacity;
    float							shininess;

    // proxies of this effect
    mutable dirty_signal            dirtyProxies;

    // maps
    sgl::ref_ptr<sgl::Texture2D>    emissionMap;
    sgl::ref_ptr<sgl::Texture2D>    diffuseSpecularMap;
    sgl::ref_ptr<sgl::Texture2D>    normalHeightMap;
};

typedef boost::intrusive_ptr<LightingEffect>        lighting_effect_ptr;
typedef boost::intrusive_ptr<const LightingEffect>  const_lighting_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_LIGHTING_EFFECT_H__
