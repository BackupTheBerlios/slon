#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/SkyBoxEffect.h"
#include "Graphics/Effect/Detail/EffectShaderProgram.h"
#include "Graphics/Effect/Detail/FFPPass.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Log/Logger.h"

__DEFINE_LOGGER__("graphics.SkyBoxEffect")

namespace {

    static const slon::unique_string OpaquePass = slon::unique_string("OpaquePass");

} // anonymous namespace

namespace slon {
namespace graphics {

SkyBoxEffect::SkyBoxEffect()
{
    dirtyShaderTechniques();
}

void SkyBoxEffect::dirtyShaderTechniques()
{
    Renderer::RENDER_TECHNIQUE renderTechnique = currentRenderer()->getRenderTechnique();
    try
    {
        switch (renderTechnique)
        {
            case Renderer::FORWARD_RENDERING:
            {
                using detail::ForwardRenderer;

                // create binders
                viewProjectionMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&viewProjectionMatrix, 1, false) );
                environmentMapBinder.reset( new parameter_binding<sgl::TextureCube>(0, 1, false) );

                EffectShaderProgram program(logger);
                program.addShader("Data/Shaders/skybox.vert");
                program.addShader("Data/Shaders/skybox.frag");

                detail::Pass::DESC desc;
                desc.program  = program.getProgram();
                desc.priority = ForwardRenderer::makePriority(ForwardRenderer::OPAQUE_BIN, desc.program);

                detail::Pass::UNIFORM_DESC uniformDesc[2];
				{
					uniformDesc[0].uniformName = "environmentMap";
					uniformDesc[0].parameter   = environmentMapBinder.get();
					uniformDesc[1].uniformName = "viewProjectionMatrix";
					uniformDesc[1].parameter   = viewProjectionMatrixBinder.get();

					desc.uniforms    = uniformDesc;
					desc.numUniforms = 2;
				}

                pass.reset( new detail::Pass(desc) );

                break;
            }

            case Renderer::FIXED_PIPELINE:
            {
                myProjectionMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&viewProjectionMatrix, 1, false) );
                environmentMapBinder2.reset( new parameter_binding<sgl::Texture>(0, 1, false) );

                detail::FFPPass::DESC desc;
                desc.projectionMatrixBinding.parameter  = myProjectionMatrixBinder.get();
                desc.worldViewMatrixBinding.parameter   = worldViewMatrixBinder.get();
                desc.textureParameters[0]               = environmentMapBinder2.get();
                pass.reset( new detail::FFPPass(desc) );

                break;
            }

            case Renderer::DEFERRED_SHADING:
                pass.reset();
                logger << log::WL_ERROR << "Deferred renderer is not supported by sky box effect." << std::endl;
                break;
        }
    }
    catch(...)
    {
        pass.reset();
        // warn here
    }
}

int SkyBoxEffect::present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes)
{
    if ( (renderGroup == detail::ForwardRenderer::mainGroupHandle() && renderPass == detail::ForwardRenderer::opaquePassHandle())
         || (renderGroup == detail::FixedPipelineRenderer::mainGroupHandle() && renderPass == detail::FixedPipelineRenderer::opaquePassHandle()) )
    {
        if (myProjectionMatrixBinder)
        {
            *worldViewMatrixBinder      = viewMatrixBinder->value();
            worldViewMatrix[0][3]       = worldViewMatrix[1][3] = worldViewMatrix[2][3] = 0.0f;

            *myProjectionMatrixBinder   = projectionMatrixBinder->value();
            viewProjectionMatrix[2]     = viewProjectionMatrix[3]; // make z and w same
        }
        else
        {
            *viewProjectionMatrixBinder = viewMatrixBinder->value();
            viewProjectionMatrix[0][3]  = viewProjectionMatrix[1][3] = viewProjectionMatrix[2][3] = 0.0f;
            viewProjectionMatrix        = projectionMatrixBinder->value() * viewProjectionMatrix;
        }

        passes[0] = pass.get();
        return 1;
    }

    return 0;
}


void SkyBoxEffect::setEnvironmentMap(sgl::TextureCube* _environmentMap)
{
    environmentMap.reset(_environmentMap);
    if (environmentMapBinder) environmentMapBinder->switch_values(environmentMap.get(), 1, false);
    if (environmentMapBinder2) environmentMapBinder2->switch_values(environmentMap.get(), 1, false);
}

} // namesapce slon
} // namespace graphics