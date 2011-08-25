#include "stdafx.h"
#include "Graphics/LightingEffect.h"
#include "PyEffect.h"

using namespace boost::python;
using namespace slon::graphics;

effect_ptr createLightingEffect(const math::Vector3f& diffuse, float shininess)
{
    lighting_material_ptr material(new LightingMaterial);
    material->setDiffuseSpecular( math::Vector4f(diffuse.x, diffuse.y, diffuse.z, shininess) );
    return material->createEffect();
}

void exportEffects()
{
    // Effect
    class_<Effect, boost::intrusive_ptr<Effect>, boost::noncopyable>("Effect", no_init);

    // LightingEffect
    def("createLightingEffect", createLightingEffect);
}