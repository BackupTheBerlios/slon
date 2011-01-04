#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/DebugEffect.h"
#include "Graphics/Effect/Detail/EffectShaderProgram.h"
#include "Graphics/Effect/Detail/FFPPass.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Log/Logger.h"
#include <sgl/Math/MatrixFunctions.hpp>

__DEFINE_LOGGER__("graphics.DebugEffect")

namespace {

    static const slon::unique_string DebugPass = slon::unique_string("DebugPass");

} // anonymous namespace

namespace slon {
namespace graphics {

// Debug effect
DebugEffect::DebugEffect(const DESC& desc_)
:   desc(desc_)
{
    // create binders
    colorBinder.reset( new parameter_binding<math::Vector4f>(&desc.color, 1, false) );
    ownProjectionMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&desc.projectionMatrix, 1, false) );

    // create pass
    switch ( currentRenderer()->getRenderTechnique() )
    {
        case Renderer::FIXED_PIPELINE:
        {
            detail::FFPPass::DESC pdesc;

            pdesc.projectionMatrixBinding.parameter  = ownProjectionMatrixBinder.get();
            pdesc.worldViewMatrixBinding.parameter   = worldViewMatrixBinder.get();
            pdesc.diffuseSpecularBinding.parameter   = colorBinder.get();

            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = desc.depthTest;
                dsDesc.stencilEnable = false;

                pdesc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);
            }

            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = desc.wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::NONE;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;

                pdesc.rasterizerState = currentDevice()->CreateRasterizerState(rsDesc);
            }

            pass.reset( new detail::FFPPass(pdesc) );
            break;
        }

        case Renderer::FORWARD_RENDERING:
        {
            using detail::ForwardRenderer;

            EffectShaderProgram program(logger);
            program.addShader("Data/Shaders/debug.vert");
            program.addShader("Data/Shaders/debug.frag");

            detail::Pass::DESC pdesc;
            pdesc.program = program.getProgram();
            pdesc.priority = ForwardRenderer::makePriority(ForwardRenderer::OPAQUE_BIN, pdesc.program);

            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = desc.depthTest;
                dsDesc.stencilEnable = false;

                pdesc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);
            }

            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = desc.wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::NONE;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;

                pdesc.rasterizerState = currentDevice()->CreateRasterizerState(rsDesc);
            }

            detail::Pass::UNIFORM_DESC uniformDesc;
            uniformDesc.uniformName = "worldViewProjMatrix";
            uniformDesc.parameter   = worldViewProjMatrixBinder.get();
            pdesc.uniforms.push_back(uniformDesc);
            uniformDesc.uniformName = "color";
            uniformDesc.parameter   = colorBinder.get();
            pdesc.uniforms.push_back(uniformDesc);

            pass.reset( new detail::Pass(pdesc) );
            break;
        }

        default:
            throw slon_error(logger, "Debug effect doesn't support provided renderer type");
    }
}

int DebugEffect::present(render_group_handle /*renderGroup*/, render_pass_handle renderPass, Pass** passes)
{
    if (renderPass == DebugPass)
    {
        if (desc.useCameraProjection)
        {
            worldViewMatrixBinder->write_value( viewMatrixBinder->value() *
                                                worldMatrixBinder->value()
                                                * desc.modelMatrix );

            worldViewProjMatrixBinder->write_value( projectionMatrixBinder->value() 
                                                    * viewMatrixBinder->value()
                                                    * worldMatrixBinder->value()
                                                    * desc.modelMatrix );

            ownProjectionMatrixBinder->write_value( projectionMatrixBinder->value() );
        }
        else
        {
            worldViewMatrixBinder->write_value( worldMatrixBinder->value()
                                                * desc.modelMatrix );

            worldViewProjMatrixBinder->write_value( desc.projectionMatrix 
                                                    * worldMatrixBinder->value()
                                                    * desc.modelMatrix );

            ownProjectionMatrixBinder->write_value( desc.projectionMatrix );
        }

        passes[0] = pass.get();
        return 1;
    }

    return 0;
}

} // namesapce slon
} // namespace graphics
