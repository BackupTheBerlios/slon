#include "Graphics/Effect/ExtrudeEffect.h"
#include "Graphics/Effect/Detail/ExtrudeTechnique.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPass.h"
#include <boost/bind.hpp>

__DEFINE_LOGGER__("graphics.ExtrudeEffect")

namespace slon {
namespace graphics {

// Extrude Effect
void ExtrudeEffect::dirtyShaderTechniques()
{
    std::for_each( techniques, 
                   techniques + scene::Light::NUM_LIGHT_TYPES,
                   boost::bind(&technique_ptr::reset, _1, (Technique*)0) );

    Renderer::RENDER_TECHNIQUE renderTechnique = currentGraphicsManager().getRenderer()->getRenderTechnique();
    try
    {
        switch (renderTechnique)
        {
        case Renderer::FORWARD_RENDERING:
            techniques[scene::Light::DIRECTIONAL].reset( new ExtrudeTechnique<scene::Light::DIRECTIONAL>(this) ); 
            techniques[scene::Light::POINT].reset( new ExtrudeTechnique<scene::Light::POINT>(this) ); 
            break; 

        case Renderer::FIXED_PIPELINE:
            logger << log::WL_ERROR << "Fixed pipeline renderer is not supported by shadow volume extrude effect." << std::endl;
            break;

        case Renderer::DEFERRED_SHADING:
            logger << log::WL_ERROR << "Deferred renderer is not supported by shadow volume extrude effect." << std::endl;
            break;
        }
    }
    catch(slon_error&)
    {
        logger << log::WL_ERROR << "Can't create lighting effect." << std::endl;
    }
}

ExtrudeEffect::ExtrudeEffect()
{
    dirtyShaderTechniques();
}

Technique* ExtrudeEffect::present(const RenderPass& pass)
{
    if ( pass.getPassSemantic() == RenderPass::SHADOW_PASS ) 
    {
        const ShadowVolumePass& sPass = static_cast<const ShadowVolumePass&>(pass);
        return techniques[sPass.getLightType()].get();
    }

    return 0;
}

} // namespace graphics
} // namespace slon