#include "stdafx.h"
#include "Scene/Light/PointLight.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Scene/Visitors/CullVisitor.h"

using namespace slon;
using namespace scene;

void PointLight::accept(scene::TraverseVisitor& visitor)
{
    position = math::make_vec(math::get_translation( visitor.getLocalToWorldTransform() ), 1.0f);
    visitor.visitEntity(*this);
}

void PointLight::accept(scene::CullVisitor& visitor)
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