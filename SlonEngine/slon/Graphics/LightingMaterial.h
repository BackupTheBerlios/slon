#ifndef __SLON_ENGINE_GRAPHICS_MATERIAL_LIGHTING_MATERIAL_H__
#define __SLON_ENGINE_GRAPHICS_MATERIAL_LIGHTING_MATERIAL_H__

#include "Material.h"
#include "ParameterBinding.h"
#include <boost/signal.hpp>
#include <sgl/Texture2D.h>

namespace slon {
namespace graphics {

/** Effect that generalizes different lighting techniques */
class SLON_PUBLIC LightingMaterial :
    public Material
{
friend class LightingEffect;
public:
    typedef boost::signal<void (const LightingMaterial*)>       dirty_signal;

public:
    LightingMaterial();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    /** Setup emission material color for the effect */
    void setEmission(const math::Vector4f& emission);

    /** Setup diffuse material color for the effect */
    void setDiffuseSpecular(const math::Vector4f& diffuseSpecular);

    /** Setup material shininess for the effect */
    void setShininess(float shininess);

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
    Effect* createEffect() const;

private:
    // properties
    math::Vector4f			emission;
    math::Vector4f			diffuseSpecular;
    float                   opacity;
    float					shininess;

    // proxies of this effect
    mutable dirty_signal    dirtyProxies;

    // maps
    texture_2d_ptr          emissionMap;
    texture_2d_ptr          diffuseSpecularMap;
    texture_2d_ptr          normalHeightMap;

    // propreties binders
    binding_vec4f_ptr       emissionBinder;
    binding_vec4f_ptr       diffuseSpecularBinder;
    binding_float_ptr       shininessBinder;
    binding_tex_2d_ptr      emissionMapBinder;
    binding_tex_2d_ptr      diffuseSpecularMapBinder;
    binding_tex_2d_ptr      normalHeightMapBinder;
};

typedef boost::intrusive_ptr<LightingMaterial>          lighting_material_ptr;
typedef boost::intrusive_ptr<const LightingMaterial>    const_lighting_material_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_MATERIAL_LIGHTING_MATERIAL_H__
