#include "stdafx.h"
#include "Scene/PointLight.h"
#include "Scene/TransformVisitor.h"
#include "Scene/CullVisitor.h"

namespace slon {
namespace scene {

void PointLight::accept(scene::TransformVisitor& visitor)
{
    const math::Matrix4f& trans = visitor.getLocalToWorldTransform();
    position.x = trans[0][3];
    position.y = trans[1][3];
    position.z = trans[2][3];
}

void PointLight::accept(scene::CullVisitor& visitor) const
{
    visitor.addLight(this);
}

void PointLight::setColor(const math::Vector4f& _color)
{
    color = math::normalize(_color);
}

void PointLight::setIntensity(float _intensity)
{
    intensity = _intensity;
}

void PointLight::setRadius(float _radius)
{
    radius = _radius;
    aabb   = math::AABBf(-radius, -radius, -radius, radius, radius, radius);
}

} // namespace slon
} // namespace scene