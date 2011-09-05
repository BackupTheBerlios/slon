#ifndef SLON_ENGINE_GRAPHICS_LIGHT_PHONG_LIGHT_H
#define SLON_ENGINE_GRAPHICS_LIGHT_PHONG_LIGHT_H

#include "Light.h"

namespace slon {
namespace scene {

/** Spherical point light source. */
class SLON_PUBLIC PointLight :
    public Light
{
public:
    // Override Entity
    using Entity::accept;

    void accept(scene::TransformVisitor& visitor);
    void accept(scene::CullVisitor& visitor) const;

    // Oveerride Entity
    const math::AABBf& getBounds() const { return aabb; }

    // Override Light
    LIGHT_TYPE getLightType() const { return POINT; }

    /** Set normalized color of the light source. */
    void setColor(const math::Vector4f& color);

    /** Set intensity of the light source. */
    void setIntensity(float intensity);

    /** Set radius of the light source. */
    void setRadius(float radius);

    /** Get light position. */
    const math::Vector4f& getPosition() const { return position; }

    /** Get normalized color of the light source. */
    const math::Vector4f& getColor() const { return color; }

    /** Get intensity of the light source. */
    float getIntensity() const { return intensity; }

    /** Get radius of the light source. */
    float getRadius() const { return radius; }

private:
    // light props
    math::Vector4f  color;
    float           intensity;

    // spatial props
    math::Vector4f  position;
    math::AABBf     aabb;
    float           radius;
};

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_GRAPHICS_LIGHT_PHONG_LIGHT_H
