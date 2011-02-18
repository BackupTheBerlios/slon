#include "stdafx.h"
#include "Scene/Light/PointLight.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Scene/Visitors/CullVisitor.h"

namespace slon {
namespace scene {

void PointLight::accept(scene::TransformVisitor& visitor)
{
    position = math::make_vec(math::get_translation( visitor.getLocalToWorldTransform() ), 1.0f);
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