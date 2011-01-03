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
DebugEffect::DebugEffect() :
    modelMatrix( math::make_identity<float, 4>() ),
    color( math::Vector4f(1.0f, 1.0f, 0.0f, 1.0f) ),
    useCameraProjection(true),
    depthTest(true)
{
    // create binders
    colorBinder.reset( new parameter_binding<math::Vector4f>(&color, 1, false) );
    ownProjectionMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&projectionMatrix, 1, false) );

    // create pass
    switch ( currentRenderer()->getRenderTechnique() )
    {
        case Renderer::FIXED_PIPELINE:
        {
            detail::FFPPass::DESC desc;

            desc.projectionMatrixBinding.parameter  = ownProjectionMatrixBinder.get();
            desc.worldViewMatrixBinding.parameter   = worldViewMatrixBinder.get();
            desc.diffuseSpecularBinding.parameter   = colorBinder.get();

            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = depthTest;
                dsDesc.stencilEnable = false;

                desc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);
            }

            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::BACK;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;

                desc.rasterizerState = currentDevice()->CreateRasterizerState(rsDesc);
            }

            pass.reset( new detail::FFPPass(desc) );
            break;
        }

        case Renderer::FORWARD_RENDERING:
        {
            using detail::ForwardRenderer;

            EffectShaderProgram program(logger);
            program.addShader("Data/Shaders/debug.vert");
            program.addShader("Data/Shaders/debug.frag");

            detail::Pass::DESC desc;
            desc.program = program.getProgram();
            desc.priority = ForwardRenderer::makePriority(ForwardRenderer::OPAQUE_BIN, desc.program);

            {
                sgl::DepthStencilState::DESC dsDesc;
                dsDesc.depthEnable   = depthTest;
                dsDesc.stencilEnable = false;

                desc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);
            }

            {
                sgl::RasterizerState::DESC rsDesc;
                rsDesc.fillMode  = wireframe ? sgl::RasterizerState::WIREFRAME : sgl::RasterizerState::SOLID;
                rsDesc.cullMode  = sgl::RasterizerState::BACK;
                rsDesc.colorMask = sgl::RasterizerState::RGBA;

                desc.rasterizerState = currentDevice()->CreateRasterizerState(rsDesc);
            }

            detail::Pass::UNIFORM_DESC uniformDesc;
            uniformDesc.uniformName = "worldViewProjMatrix";
            uniformDesc.parameter   = worldViewProjMatrixBinder.get();
            desc.uniforms.push_back(uniformDesc);
            uniformDesc.uniformName = "color";
            uniformDesc.parameter   = colorBinder.get();
            desc.uniforms.push_back(uniformDesc);

            pass.reset( new detail::Pass(desc) );
            break;
        }

        default:
            throw slon_error(logger, "Debug effect doesn't support provided renderer type");
    }
}

void DebugEffect::setModelMatrix(const math::Matrix4f& _modelMatrix)
{
    modelMatrix = _modelMatrix;
}

void DebugEffect::setProjectionMatrix(bool useCameraProjection_, const math::Matrix4f& projectionMatrix_)
{
    useCameraProjection = useCameraProjection_;
    projectionMatrix    = projectionMatrix_;
}

void DebugEffect::setColor(const math::Vector4f& color)
{
    colorBinder->write_value(color);
}

void DebugEffect::toggleDepthTest(bool _depthTest)
{
    depthTest = _depthTest;
}

void DebugEffect::toggleWireframe(bool _wireframe)
{
    wireframe = _wireframe;
}

int DebugEffect::present(render_group_handle /*renderGroup*/, render_pass_handle renderPass, Pass** passes)
{
    if (renderPass == DebugPass)
    {
        if (useCameraProjection)
        {
            worldViewMatrixBinder->write_value( viewMatrixBinder->value() *
                                                worldMatrixBinder->value()
                                                * modelMatrix );

            worldViewProjMatrixBinder->write_value( projectionMatrixBinder->value() 
                                                    * viewMatrixBinder->value()
                                                    * worldMatrixBinder->value()
                                                    * modelMatrix );

            ownProjectionMatrixBinder->write_value( projectionMatrixBinder->value() );
        }
        else
        {
            worldViewMatrixBinder->write_value( worldMatrixBinder->value()
                                                * modelMatrix );

            worldViewProjMatrixBinder->write_value( projectionMatrix 
                                                    * worldMatrixBinder->value()
                                                    * modelMatrix );

            ownProjectionMatrixBinder->write_value( projectionMatrix );
        }

        passes[0] = pass.get();
        return 1;
    }

    return 0;
}

} // namesapce slon
} // namespace graphics
