#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Detail/FFPPass.h"
#include "Log/Logger.h"

DECLARE_AUTO_LOGGER("graphics.detail.FFPPass")

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
        if (desc.projectionMatrixBinding.parameter || desc.projectionMatrixBinding.parameterName)
        {
            projectionMatrixParameter.reset( desc.projectionMatrixBinding.parameter ?
                                             desc.projectionMatrixBinding.parameter :
                                             pTable.getParameterBinding<math::Matrix4f>( hash_string(desc.projectionMatrixBinding.parameterName) ).get() );
        }
        if (desc.worldViewMatrixBinding.parameter || desc.worldViewMatrixBinding.parameterName)
        {
            worldViewMatrixParameter.reset( desc.worldViewMatrixBinding.parameter ?
                                            desc.worldViewMatrixBinding.parameter :
                                            pTable.getParameterBinding<math::Matrix4f>( hash_string(desc.worldViewMatrixBinding.parameterName) ).get() );
        }
        if (desc.diffuseSpecularBinding.parameter || desc.diffuseSpecularBinding.parameterName)
        {
            diffuseSpecularParameter.reset( desc.diffuseSpecularBinding.parameter ?
                                            desc.diffuseSpecularBinding.parameter :
                                            pTable.getParameterBinding<math::Vector4f>( hash_string(desc.diffuseSpecularBinding.parameterName) ).get() );
        }
        if (desc.shininessBinding.parameter || desc.shininessBinding.parameterName)
        {
            shininessParameter.reset( desc.shininessBinding.parameter ?
                                      desc.shininessBinding.parameter :
                                      pTable.getParameterBinding<float>( hash_string(desc.shininessBinding.parameterName) ).get() );
        }
    }

    std::copy( desc.textureParameters, 
               desc.textureParameters + 8, 
               textureParameters.begin() );
}
	
void FFPPass::setBlendState(const sgl::BlendState* blendState_)
{
	blendState.reset(blendState_);
}

void FFPPass::setDepthStencilState(const sgl::DepthStencilState* depthStencilState_)
{
	depthStencilState.reset(depthStencilState_);
}

void FFPPass::setRasterizerState(const sgl::RasterizerState* rasterizerState_)
{
	rasterizerState.reset(rasterizerState_);
}

void FFPPass::setProjectionMatrixBinder(const PARAMETER_DESC<math::Matrix4f>& binding)
{
    projectionMatrixParameter.reset( binding.parameter ?
                                     binding.parameter :
                                     currentParameterTable().getParameterBinding<math::Matrix4f>( hash_string(binding.parameterName) ).get() );
}

void FFPPass::setWorldViewMatrixBinder(const PARAMETER_DESC<math::Matrix4f>& binding)
{
    worldViewMatrixParameter.reset( binding.parameter ?
                                    binding.parameter :
                                    currentParameterTable().getParameterBinding<math::Matrix4f>( hash_string(binding.parameterName) ).get() );
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
