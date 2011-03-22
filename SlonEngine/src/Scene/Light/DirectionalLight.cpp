#include "stdafx.h"
#include "Scene/Light/DirectionalLight.h"
#include "Scene/Visitors/CullVisitor.h"

namespace slon {
namespace scene {

void DirectionalLight::accept(scene::CullVisitor& visitor) const
{
    visitor.addLight(this);
}

DirectionalLight::DirectionalLight() :
    color(1.0f, 1.0f, 1.0f, 1.0f),
    direction(0.0f, -1.0f, 0.0f),
    intensity(1.0f),
    ambient(0.1f),
    shadowCaster(false)
{
}

} // namespaec scene
} // namespace slon
