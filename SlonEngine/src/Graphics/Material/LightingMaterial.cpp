#include "stdafx.h"
#include "Graphics/Effect/LightingEffect.h"
#include "Graphics/Material/LightingMaterial.h"

namespace slon {
namespace graphics {

// Lighting effect
LightingMaterial::LightingMaterial() :
    diffuseSpecular(0.5f, 0.5f, 0.5f, 0.5f),
    opacity(1.0f),
    shininess(1.0f)
{
    // create binders
    emissionBinder.reset( new parameter_binding<math::Vector4f>(&emission, 1, false) );
    diffuseSpecularBinder.reset( new parameter_binding<math::Vector4f>(&diffuseSpecular, 1, false) );
    shininessBinder.reset( new parameter_binding<float>(&shininess, 1, false) );
    emissionMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
    diffuseSpecularMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
    normalHeightMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
}

void LightingMaterial::setEmission(const math::Vector4f& emission_)               
{ 
    emissionBinder->write_value(emission_); 
}

void LightingMaterial::setDiffuseSpecular(const math::Vector4f& diffuseSpecular_) 
{ 
    diffuseSpecularBinder->write_value(diffuseSpecular_); 
}

void LightingMaterial::setShininess(float shininess_)                             
{ 
    shininessBinder->write_value(shininess_); 
}

void LightingMaterial::setEmissionMap(sgl::Texture2D* _emissionMap)
{
    emissionMap.reset(_emissionMap);
    emissionMapBinder->switch_values(emissionMap.get(), 1, false);
    dirtyProxies(this);
}

void LightingMaterial::setDiffuseSpecularMap(sgl::Texture2D* _diffuseSpecularMap)
{
    diffuseSpecularMap.reset(_diffuseSpecularMap);
    diffuseSpecularMapBinder->switch_values(diffuseSpecularMap.get(), 1, false);
    dirtyProxies(this);
}

void LightingMaterial::setNormalHeightMap(sgl::Texture2D* _normalHeightMap)
{
    normalHeightMap.reset(_normalHeightMap);
    normalHeightMapBinder->switch_values(normalHeightMap.get(), 1, false);
    dirtyProxies(this);
}

void LightingMaterial::setOpacity(float _opacity)
{
    opacity = _opacity;
    dirtyProxies(this);
}

Effect* LightingMaterial::createEffect() const
{
    return new LightingEffect(this, dirtyProxies);
}

} // namesapce slon
} // namespace graphics
