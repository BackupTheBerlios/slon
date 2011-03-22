#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/DebugEffect.h"
#include "Graphics/Effect/Detail/EffectShaderProgram.h"
#include "Graphics/Effect/Detail/FFPPass.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Log/Logger.h"
#include <sgl/Math/MatrixFunctions.hpp>

__DEFINE_LOGGER__("graphics.DebugEffect")

namespace {

    static const slon::hash_string DebugPass = slon::hash_string("DebugPass");

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

            pdesc.projectionMatrixBinding.parameter = ownProjectionMatrixBinder.get();
            pdesc.worldViewMatrixBinding.parameter  = worldViewMatrixBinder.get();
            pdesc.diffuseSpecularBinding.parameter  = colorBinder.get();

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
            pdesc.program  = program.getProgram();
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

            detail::Pass::UNIFORM_DESC uniformDesc[2];
			{
				uniformDesc[0].uniformName = "worldViewProjMatrix";
				uniformDesc[0].parameter   = worldViewProjMatrixBinder.get();
				uniformDesc[1].uniformName = "color";
				uniformDesc[1].parameter   = colorBinder.get();
			}
            pdesc.uniforms    = uniformDesc;
			pdesc.numUniforms = 2;

            pass.reset( new detail::Pass(pdesc) );
            break;
        }

        default:
            throw slon_error(logger, "Debug effect doesn't support provided renderer type");
    }
}

DebugEffect::DebugEffect(const DebugEffect& effect)
:   desc(effect.desc)
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
			detail::FFPPass&      fpass = static_cast<detail::FFPPass&>(*effect.pass);

            pdesc.projectionMatrixBinding.parameter = ownProjectionMatrixBinder.get();
            pdesc.worldViewMatrixBinding.parameter  = worldViewMatrixBinder.get();
            pdesc.diffuseSpecularBinding.parameter  = colorBinder.get();
			pdesc.depthStencilState                 = fpass.getDepthStencilState();
			pdesc.rasterizerState                   = fpass.getRasterizerState();

            pass.reset( new detail::FFPPass(pdesc) );
            break;
        }

        case Renderer::FORWARD_RENDERING:
        {
            detail::Pass::DESC pdesc;
			detail::Pass&      dpass = static_cast<detail::Pass&>(*effect.pass);

            pdesc.program            = dpass.getProgram();
            pdesc.priority           = dpass.getPriority();
			pdesc.depthStencilState  = dpass.getDepthStencilState();
			pdesc.rasterizerState    = dpass.getRasterizerState();

            detail::Pass::UNIFORM_DESC uniformDesc[2];
			{
				uniformDesc[0].uniform   = dpass.getUniformBinder(0)->getBindingBase()->get_uniform_base();
				uniformDesc[0].parameter = worldViewProjMatrixBinder.get();
				uniformDesc[1].uniform   = dpass.getUniformBinder(1)->getBindingBase()->get_uniform_base();
				uniformDesc[1].parameter = colorBinder.get();
			}
            pdesc.uniforms    = uniformDesc;
			pdesc.numUniforms = 2;

            pass.reset( new detail::Pass(pdesc) );
            break;
        }

        default:
            throw slon_error(logger, "Debug effect doesn't support provided renderer type");
    }
}

void DebugEffect::reset(const DESC& desc_)
{
    // create pass
    switch ( currentRenderer()->getRenderTechnique() )
    {
        case Renderer::FIXED_PIPELINE:
        {
			detail::FFPPass& fPass = static_cast<detail::FFPPass&>(*pass);
			
			if (desc.depthTest != desc_.depthTest)
            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = desc_.depthTest;
                dsDesc.stencilEnable = false;
                fPass.setDepthStencilState( currentDevice()->CreateDepthStencilState(dsDesc) );
            }

			if (desc.wireframe != desc_.wireframe)
            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = desc_.wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::NONE;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;
                fPass.setRasterizerState( currentDevice()->CreateRasterizerState(rsDesc) );
            }

            break;
        }

        case Renderer::FORWARD_RENDERING:
        {
			detail::Pass& dPass = static_cast<detail::Pass&>(*pass);
			
			if (desc.depthTest != desc_.depthTest)
            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = desc_.depthTest;
                dsDesc.stencilEnable = false;
                dPass.setDepthStencilState( currentDevice()->CreateDepthStencilState(dsDesc) );
            }

			if (desc.wireframe != desc_.wireframe)
            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = desc_.wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::NONE;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;
                dPass.setRasterizerState( currentDevice()->CreateRasterizerState(rsDesc) );
            }

            break;
        }

        default:
            throw slon_error(logger, "Debug effect doesn't support provided renderer type");
    }

	desc = desc_;
}

int DebugEffect::present(render_group_handle /*renderGroup*/, render_pass_handle renderPass, Pass** passes)
{
    if (renderPass == detail::ForwardRenderer::RP_DEBUG || renderPass == detail::FixedPipelineRenderer::RP_DEBUG)
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
