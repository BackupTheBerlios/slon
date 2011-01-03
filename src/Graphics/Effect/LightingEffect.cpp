#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/LightingEffect.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Effect/Detail/FFPPass.h"
#include "Graphics/Detail/AttributeTable.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include <boost/bind.hpp>
#include <sgl/Math/MatrixFunctions.hpp>

__DEFINE_LOGGER__("graphics.LightingEffect")

namespace slon {
namespace graphics {

LightingEffect::LightingEffect(const LightingMaterial*          material_,
                               LightingMaterial::dirty_signal&  dirtySignal)
:   opacity(1.0f)
,   material(material_)
{
    opacityOneBinder.reset( new binding_float(&opacity, 1, false) );

    // bind parameters
    detail::ParameterTable& parameterTable = detail::currentParameterTable();
    lightCountBinder = parameterTable.getParameterBinding<int>( unique_string("lightCount") );

    dirtyConnection = dirtySignal.connect( boost::bind(&LightingEffect::dirty, this, _1) );
    dirty(material_);
}

// Override Effect
int LightingEffect::present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** p)
{
	if (isDirty) {
		dirty(material.get());
	}

    if ( renderPass == detail::FixedPipelineRenderer::lightingPassHandle() )
    {
        worldViewMatrixBinder->write_value( viewMatrixBinder->value() * worldMatrixBinder->value() );
        p[0] = ffpPass.get();
        return 1;
    }

    int    numPasses = 0;
    size_t numLights = lightCountBinder->value();
    if ( renderGroup == detail::ForwardRenderer::reflectGroupHandle() )
    {
        if (opacityBinder)
        {
            if ( renderPass == detail::ForwardRenderer::directionalLightingPassHandle() )
            {
                p[0] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                p[1] = passes[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::pointLightingPassHandle() )
            {
                p[0] = backFacePasses[scene::Light::POINT][numLights].get();
                p[1] = passes[scene::Light::POINT][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::spotLightingPassHandle() )
            {
                p[0] = backFacePasses[scene::Light::SPOT][numLights].get();
                p[1] = passes[scene::Light::SPOT][numLights].get();
                numPasses = 2;
            }
        }
        else
        {
            if ( renderPass == detail::ForwardRenderer::directionalLightingPassHandle() )
            {
                p[0] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::pointLightingPassHandle()  )
            {
                p[0] = backFacePasses[scene::Light::POINT][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::spotLightingPassHandle()  )
            {
                p[0] = backFacePasses[scene::Light::SPOT][numLights].get();
                numPasses = 1;
            }
            else {
                return TransformEffect::present(renderGroup, renderPass, p);
            }
        }
    }
    else
    {
        if (opacityBinder)
        {
            if ( renderPass == detail::ForwardRenderer::directionalLightingPassHandle() )
            {
                p[0] = passes[scene::Light::DIRECTIONAL][numLights].get();
                p[1] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::pointLightingPassHandle() )
            {
                p[0] = passes[scene::Light::POINT][numLights].get();
                p[1] = backFacePasses[scene::Light::POINT][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::spotLightingPassHandle() )
            {
                p[0] = passes[scene::Light::SPOT][numLights].get();
                p[1] = backFacePasses[scene::Light::SPOT][numLights].get();
                numPasses = 2;
            }
        }
        else
        {
            if ( renderPass == detail::ForwardRenderer::directionalLightingPassHandle() )
            {
                p[0] = passes[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::pointLightingPassHandle()  )
            {
                p[0] = passes[scene::Light::POINT][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::spotLightingPassHandle()  )
            {
                p[0] = passes[scene::Light::SPOT][numLights].get();
                numPasses = 1;
            }
            else {
                return TransformEffect::present(renderGroup, renderPass, p);
            }
        }
    }

    if (numPasses > 0)
    {
        worldViewMatrixBinder->write_value( viewMatrixBinder->value() * worldMatrixBinder->value() );
        normalMatrixBinder->write_value( math::get_rotation( math::transpose( math::invert( worldViewMatrixBinder->value() ) ) ) );
        worldViewProjMatrixBinder->write_value( projectionMatrixBinder->value() * viewMatrixBinder->value() * worldMatrixBinder->value() );
    }

    return numPasses;
}

const abstract_parameter_binding* LightingEffect::getParameter(unique_string name) const
{
    if ( name == unique_string("opacity") ) {
        return opacityBinder.get();
    }

    return base_type::getParameter(name);
}

bool LightingEffect::bindParameter(unique_string                        name,
                                   const abstract_parameter_binding*    binding)
{
    if ( name == unique_string("opacity") && ((opacityBinder = cast_binding<float>(binding)) || !binding) )
    {
        isDirty = true;
        return true;
    }
    else if ( base_type::bindParameter(name, binding) ) {
        return true;
    }

    return false;
}

int LightingEffect::queryAttribute(unique_string name)
{
    return detail::currentAttributeTable().queryAttribute(name)->index;
}

void LightingEffect::dirty(const LightingMaterial* material_)
{
    // remove techniques
    material.reset(material_);
    for (int i = 0; i<scene::Light::NUM_LIGHT_TYPES; ++i)
    {
        for (int j = MIN_NUM_LIGHTS; j < MAX_NUM_LIGHTS; ++j) {
            passes[i][j].reset();
        }
    }

    // create technique accordingly to renderer
    Renderer::RENDER_TECHNIQUE technique = currentRenderer()->getRenderTechnique();
    try
    {
        switch (technique)
        {
            case Renderer::FORWARD_RENDERING:
            {
                using detail::ForwardRenderer;

                detail::Pass::DESC         desc;
                detail::Pass::UNIFORM_DESC uniformDesc;
                {
                    uniformDesc.uniformName   = "projectionMatrix";
                    uniformDesc.parameterName = "projectionMatrix";
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "lightColorIntensity";
                    uniformDesc.parameterName = "lightColorIntensity";
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "lightDirectionAmbient";
                    uniformDesc.parameterName = "lightViewDirectionAmbient";
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "lightPositionRadius";
                    uniformDesc.parameterName = "lightViewPositionRadius";
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "materialDiffuseSpecular";
                    uniformDesc.parameter     = material->diffuseSpecularBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "materialShininess";
                    uniformDesc.parameter     = material->shininessBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "normalMatrix";
                    uniformDesc.parameter     = normalMatrixBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "worldViewMatrix";
                    uniformDesc.parameter     = worldViewMatrixBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "worldViewProjMatrix";
                    uniformDesc.parameter     = worldViewProjMatrixBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "diffuseSpecularMap";
                    uniformDesc.parameter     = material->diffuseSpecularMapBinder.get();
                    desc.uniforms.push_back(uniformDesc);
                    uniformDesc.uniformName   = "opacity";
                    uniformDesc.parameter     = opacityBinder ? opacityBinder.get() : opacityOneBinder.get();
                    desc.uniforms.push_back(uniformDesc);

					if (boneMatricesBinder) 
					{
						uniformDesc.uniformName = "boneMatrices";
						uniformDesc.parameter   = boneMatricesBinder.get();
						desc.uniforms.push_back(uniformDesc);
					}

                    if (boneRotationsBinder)
                    {
				        uniformDesc.uniformName = "boneRotations";
				        uniformDesc.parameter   = boneRotationsBinder.get();
				        desc.uniforms.push_back(uniformDesc);
                    }

                    if (boneTranslationsBinder)
                    {
				        uniformDesc.uniformName = "boneTranslations";
				        uniformDesc.parameter   = boneTranslationsBinder.get();
				        desc.uniforms.push_back(uniformDesc);
                    }
                }

                // create passes
                EffectShaderProgram baseProgram(logger);
				if (boneMatricesBinder || (boneRotationsBinder && boneTranslationsBinder)) {
					baseProgram.addShader("Data/Shaders/skinned.vert");
				}
				else {
					baseProgram.addShader("Data/Shaders/rigid.vert");
				}
                baseProgram.addShader("Data/Shaders/Forward/normal.frag");
                baseProgram.addShader("Data/Shaders/Forward/lighting.frag");
                if (material->diffuseSpecularMap) {
                    baseProgram.addDefinition("#define ENABLE_DIFFUSE_SPECULAR_MAP");
                }

                // make states
                sgl::BlendState::DESC blendDesc;
                {
                    blendDesc.blendEnable    = true;
                    blendDesc.blendOp        = sgl::BlendState::ADD;
                    blendDesc.blendOpAlpha   = sgl::BlendState::ADD;
                    if (opacityBinder)
                    {
                        blendDesc.srcBlend       = sgl::BlendState::SRC_ALPHA;
                        blendDesc.srcBlendAlpha  = sgl::BlendState::SRC_ALPHA;
                        blendDesc.destBlend      = sgl::BlendState::ONE_MINUS_SRC_ALPHA;
                        blendDesc.destBlendAlpha = sgl::BlendState::ONE_MINUS_SRC_ALPHA;
                    }
                    else
                    {
                        blendDesc.srcBlend       = sgl::BlendState::ONE;
                        blendDesc.srcBlendAlpha  = sgl::BlendState::ONE;
                        blendDesc.destBlend      = sgl::BlendState::ONE;
                        blendDesc.destBlendAlpha = sgl::BlendState::ONE;
                    }
                }
                desc.blendState = currentDevice()->CreateBlendState(blendDesc);

                sgl::DepthStencilState::DESC dsDesc;
                {
                    dsDesc.depthEnable    = true;
                    dsDesc.depthFunc      = sgl::DepthStencilState::LEQUAL;
                    dsDesc.stencilEnable  = false;
                    dsDesc.depthWriteMask = opacityBinder ? true : false;
                }
                desc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);

                sgl::RasterizerState::DESC rastDesc;
                {
                    rastDesc.cullMode  = sgl::RasterizerState::BACK;
                    rastDesc.fillMode  = sgl::RasterizerState::SOLID;
                    rastDesc.colorMask = sgl::RasterizerState::RGBA;
                }
                desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);

                ForwardRenderer::RENDER_BIN frontPassBin = ForwardRenderer::OPAQUE_BIN;
                ForwardRenderer::RENDER_BIN backPassBin  = ForwardRenderer::OPAQUE_BIN;
                if (opacityBinder)
                {
                    frontPassBin = ForwardRenderer::FRONT_TRANSPARENT_BIN;
                    backPassBin  = ForwardRenderer::BACK_TRANSPARENT_BIN;
                }

                for (int i = MIN_NUM_LIGHTS; i <= MAX_NUM_LIGHTS; ++i)
                {
                    EffectShaderProgram directionalProgram(baseProgram);
                    directionalProgram.addDefinition("#define DIRECTIONAL_LIGHTING");
                    directionalProgram.addDefinition("#define NUM_LIGHTS %d", i);
                    desc.program = directionalProgram.getProgram();

                    rastDesc.cullMode    = sgl::RasterizerState::BACK;
                    desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);
                    desc.priority        = ForwardRenderer::makePriority(frontPassBin, desc.program);
                    passes[scene::Light::DIRECTIONAL][i].reset( new detail::Pass(desc) );

                    rastDesc.cullMode    = sgl::RasterizerState::FRONT;
                    desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);
                    desc.priority        = ForwardRenderer::makePriority(backPassBin, desc.program);
                    backFacePasses[scene::Light::DIRECTIONAL][i].reset( new detail::Pass(desc) );

                    EffectShaderProgram pointProgram(baseProgram);
                    pointProgram.addDefinition("#define POINT_LIGHTING");
                    pointProgram.addDefinition("#define NUM_LIGHTS %d", i);
                    desc.program = pointProgram.getProgram();

                    rastDesc.cullMode    = sgl::RasterizerState::BACK;
                    desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);
                    desc.priority        = ForwardRenderer::makePriority(frontPassBin, desc.program);
                    passes[scene::Light::POINT][i].reset( new detail::Pass(desc) );

                    rastDesc.cullMode    = sgl::RasterizerState::FRONT;
                    desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);
                    desc.priority        = ForwardRenderer::makePriority(backPassBin, desc.program);
                    backFacePasses[scene::Light::POINT][i].reset( new detail::Pass(desc) );
                }

                break; 
            }

        case Renderer::FIXED_PIPELINE:
        {
            detail::FFPPass::DESC desc;
            desc.projectionMatrixBinding.parameterName = "projectionMatrix";
            desc.worldViewMatrixBinding.parameter      = worldViewMatrixBinder.get();
            desc.diffuseSpecularBinding.parameter      = material->diffuseSpecularBinder.get();
            desc.shininessBinding.parameter            = material->shininessBinder.get();
            //desc.diffuseMapBinding.parameter           = material->diffuseSpecularMapBinder.get();
            ffpPass.reset( new detail::FFPPass(desc) );

            break;
        }

        case Renderer::DEFERRED_SHADING:
            logger << log::WL_ERROR << "Deferred renderer is not supported by lighting effect." << std::endl;
            break;
        }
    }
    catch(slon_error&)
    {
        logger << log::WL_ERROR << "Can't create lighting effect." << std::endl;
    }

	TransformEffect::dirty();
}

} // namesapce slon
} // namespace graphics
