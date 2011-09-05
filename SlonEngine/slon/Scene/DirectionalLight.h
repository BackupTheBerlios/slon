#ifndef __SLON_ENGINE_GRAPHICS_LIGHT_DIRECTIONAL_H__
#define __SLON_ENGINE_GRAPHICS_LIGHT_DIRECTIONAL_H__

#include "../Utility/math.hpp"
#include "Light.h"

namespace slon {
namespace scene {

/** Directional light source. */
class SLON_PUBLIC DirectionalLight :
    public Light
{
public:
    DirectionalLight();

    // Override Node
    using Node::accept;

    void accept(CullVisitor& visitor) const;

    // Oveerride Entity
    const math::AABBf& getBounds() const    { return bounds<math::AABBf>::infinite(); }

    // Override Light
    LIGHT_TYPE getLightType() const         { return DIRECTIONAL; }
    bool isShadowCaster() const             { return shadowCaster; }

    /** Set normalized color of the light source. */
    void setColor(const math::Vector4f& _color) { color = _color; }

    /** Set nambient light intensity. */
    void setAmbient(float ambient_) { ambient = ambient_; }

    /** Set intensity of the light source. */
    void setIntensity(float _intensity) { intensity = _intensity; }

    /** Set direction of the light source. */
    void setDirection(const math::Vector3f& _direction) { direction = _direction; }

    /** Toggle shadow casting for light source. */
    void toggleShadow(bool toggle) { shadowCaster = toggle; }

    /** Get light direction. */
    const math::Vector3f& getDirection() const  { return direction; }

    /** Get normalized color of the light source. */
    const math::Vector4f& getColor() const { return color; }

    /** Get ambient of the light source. */
    float getAmbient() const { return ambient; }

    /** Get intensity of the light source. */
    float getIntensity() const { return intensity; }

private:
    // light props
    math::Vector4f  color;
    math::Vector3f  direction;
    float           intensity;
    float           ambient;
    bool            shadowCaster;
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_LIGHT_DIRECTIONAL_H__
