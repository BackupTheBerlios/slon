#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Effect/Detail/FFPPass.h"
#include "Log/Logger.h"

__DEFINE_LOGGER__("graphics.detail.FFPPass")

namespace {

    static const float defaultShininess = 1.0f;

    using namespace slon;

    sgl::ref_ptr<sgl::BlendState>           defaultBlendState;
    sgl::ref_ptr<sgl::DepthStencilState>    defaultDepthStencilState;
    sgl::ref_ptr<sgl::RasterizerState>      defaultRasterizerState;

    sgl::BlendState* createDefaultBlendState()
    {
        if (!defaultBlendState)
        {
            sgl::BlendState::DESC desc;
            desc.blendEnable = false;
            defaultBlendState.reset( graphics::currentDevice()->CreateBlendState(desc) );
        }

        return defaultBlendState.get();
    }

    sgl::DepthStencilState* createDefaultDepthStencilState()
    {
        if (!defaultDepthStencilState)
        {
            sgl::DepthStencilState::DESC desc;
            desc.depthEnable    = true;
            desc.depthFunc      = sgl::DepthStencilState::LEQUAL;
            desc.depthWriteMask = true;
            desc.stencilEnable  = false;
            defaultDepthStencilState.reset( graphics::currentDevice()->CreateDepthStencilState(desc) );
        }

        return defaultDepthStencilState.get();
    }

    sgl::RasterizerState* createDefaultRasterizerState()
    {
        if (!defaultRasterizerState)
        {
            sgl::RasterizerState::DESC desc;
            desc.cullMode  = sgl::RasterizerState::BACK;
            desc.fillMode  = sgl::RasterizerState::SOLID;
            desc.colorMask = sgl::RasterizerState::RGBA;
            defaultRasterizerState.reset( graphics::currentDevice()->CreateRasterizerState(desc) );
        }

        return defaultRasterizerState.get();
    }

} // anonymous namespace

namespace slon {
namespace graphics {
namespace detail {

FFPPass::FFPPass(const DESC& desc) :
    textureParameters(8)
{
    blendState.reset(desc.blendState ? desc.blendState : createDefaultBlendState());
    depthStencilState.reset(desc.depthStencilState ? desc.depthStencilState : createDefaultDepthStencilState());
    rasterizerState.reset(desc.rasterizerState ? desc.rasterizerState : createDefaultRasterizerState());

    detail::ParameterTable& pTable = currentParameterTable();
    {
        projectionMatrixParameter.reset( desc.projectionMatrixBinding.parameter ?
                                         desc.projectionMatrixBinding.parameter :
                                         pTable.getParameterBinding<math::Matrix4f>( unique_string(desc.projectionMatrixBinding.parameterName) ).get() );
        worldViewMatrixParameter.reset( desc.worldViewMatrixBinding.parameter ?
                                        desc.worldViewMatrixBinding.parameter :
                                        pTable.getParameterBinding<math::Matrix4f>( unique_string(desc.worldViewMatrixBinding.parameterName) ).get() );
        diffuseSpecularParameter.reset( desc.diffuseSpecularBinding.parameter ?
                                        desc.diffuseSpecularBinding.parameter :
                                        pTable.getParameterBinding<math::Vector4f>( unique_string(desc.diffuseSpecularBinding.parameterName) ).get() );
        shininessParameter.reset( desc.shininessBinding.parameter ?
                                  desc.shininessBinding.parameter :
                                  pTable.getParameterBinding<float>( unique_string(desc.shininessBinding.parameterName) ).get() );
    }

    std::copy( desc.textureParameters, 
               desc.textureParameters + 8, 
               textureParameters.begin() );
}

void FFPPass::begin() const
{
    sgl::Device*     device  = currentDevice();
    sgl::FFPProgram* program = device->FixedPipelineProgram();

    program->GetProjectionMatrixUniform()->Set( projectionMatrixParameter->value() );
    program->GetModelViewMatrixUniform()->Set( worldViewMatrixParameter->value() );

    if (diffuseSpecularParameter)
    {
        math::Vector4f diffuse = math::make_vec( math::xyz( diffuseSpecularParameter->value() ), 1.0f );
        program->GetMaterialDiffuseUniform()->Set(diffuse);
        program->GetMaterialSpecularUniform()->Set(diffuse * diffuseSpecularParameter->value().w);
    }

    for (size_t i = 0; i<textureParameters.size(); ++i)
    {
        if ( textureParameters[i] && textureParameters[i]->values() ) {
            textureParameters[i]->values()->Bind(i);
        }
    }

    if (shininessParameter) {
        program->GetMaterialShininessUniform()->Set( shininessParameter->value() );
    }

    // bind states
    if ( device->CurrentBlendState() != blendState ) {
        blendState->Bind();
    }

    if ( device->CurrentDepthStencilState() != depthStencilState ) {
        depthStencilState->Bind();
    }

    if ( device->CurrentRasterizerState() != rasterizerState ) {
        rasterizerState->Bind();
    }
}

void FFPPass::end() const
{
    for (size_t i = 0; i<textureParameters.size(); ++i)
    {
        if ( textureParameters[i] && textureParameters[i]->values() ) {
            textureParameters[i]->values()->Unbind();
        }
    }
}

} // namespace detail
} // namespace graphics
} // namespace slon
