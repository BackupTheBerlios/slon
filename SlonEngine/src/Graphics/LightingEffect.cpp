#include "stdafx.h"
#include "Database/Archive.h"
#include "Graphics/Common.h"
#include "Graphics/LightingEffect.h"
#include "Graphics/Detail/Pass.h"
#include "Graphics/Detail/FFPPass.h"
#include "Graphics/Detail/AttributeTable.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/FixedPipelineRenderer.h"
#include "Graphics/ForwardRenderer.h"
#include <boost/bind.hpp>
#include <sgl/Math/MatrixFunctions.hpp>

DECLARE_AUTO_LOGGER("graphics.LightingEffect")

namespace slon {
namespace graphics {

LightingEffect::LightingEffect()
:   opacity(1.0f)
{
    opacityOneBinder.reset( new binding_float(&opacity, 1, false) );

    // bind parameters
    detail::ParameterTable& parameterTable = detail::currentParameterTable();
    lightCountBinder = parameterTable.getParameterBinding<int>( hash_string("lightCount") );
}

LightingEffect::LightingEffect(const LightingMaterial*          material_,
                               LightingMaterial::dirty_signal&  dirtySignal)
:   opacity(1.0f)
,   material(material_)
{
    opacityOneBinder.reset( new binding_float(&opacity, 1, false) );

    // bind parameters
    detail::ParameterTable& parameterTable = detail::currentParameterTable();
    lightCountBinder = parameterTable.getParameterBinding<int>( hash_string("lightCount") );

    dirtyConnection = dirtySignal.connect( boost::bind(&LightingEffect::dirty, this, _1) );
    dirty(material_);
}

LightingEffect::~LightingEffect()
{
}

// Override Serializable
const char* LightingEffect::serialize(database::OArchive& ar) const
{
    ar.writeSerializable(material.get());
    return "LightingEffect";
}

void LightingEffect::deserialize(database::IArchive& ar)
{
    material = ar.readSerializable<LightingMaterial>(true);
    dirtyConnection = material->dirtyProxies.connect( boost::bind(&LightingEffect::dirty, this, _1) );
    dirty(material.get());
}

// Override Effect
int LightingEffect::present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** p)
{
	if (isDirty) {
		dirty(material.get());
	}
    
    if (ffpPass)
    {
        if ( renderPass == detail::FixedPipelineRenderer::RP_LIGHTING )
        {
            worldViewMatrixBinder->write_value( viewMatrixBinder->value() * worldMatrixBinder->value() );
            p[0] = ffpPass.get();
            return 1;
        }

        return 0;
    }

    int    numPasses = 0;
    size_t numLights = lightCountBinder->value();
    if ( renderGroup == detail::ForwardRenderer::RG_REFLECT )
    {
        if (opacityBinder)
        {
            if ( renderPass == detail::ForwardRenderer::RP_DIRECTIONAL_LIGHTING )
            {
                p[0] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                p[1] = passes[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_POINT_LIGHTING )
            {
                p[0] = backFacePasses[scene::Light::POINT][numLights].get();
                p[1] = passes[scene::Light::POINT][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_SPOT_LIGHTING )
            {
                p[0] = backFacePasses[scene::Light::SPOT][numLights].get();
                p[1] = passes[scene::Light::SPOT][numLights].get();
                numPasses = 2;
            }
        }
        else
        {
            if ( renderPass == detail::ForwardRenderer::RP_DIRECTIONAL_LIGHTING )
            {
                p[0] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_POINT_LIGHTING  )
            {
                p[0] = backFacePasses[scene::Light::POINT][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_SPOT_LIGHTING  )
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
            if ( renderPass == detail::ForwardRenderer::RP_DIRECTIONAL_LIGHTING )
            {
                p[0] = passes[scene::Light::DIRECTIONAL][numLights].get();
                p[1] = backFacePasses[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_POINT_LIGHTING )
            {
                p[0] = passes[scene::Light::POINT][numLights].get();
                p[1] = backFacePasses[scene::Light::POINT][numLights].get();
                numPasses = 2;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_SPOT_LIGHTING )
            {
                p[0] = passes[scene::Light::SPOT][numLights].get();
                p[1] = backFacePasses[scene::Light::SPOT][numLights].get();
                numPasses = 2;
            }
        }
        else
        {
            if ( renderPass == detail::ForwardRenderer::RP_DIRECTIONAL_LIGHTING )
            {
                p[0] = passes[scene::Light::DIRECTIONAL][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_POINT_LIGHTING )
            {
                p[0] = passes[scene::Light::POINT][numLights].get();
                numPasses = 1;
            }
            else if ( renderPass == detail::ForwardRenderer::RP_SPOT_LIGHTING )
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

const abstract_parameter_binding* LightingEffect::getParameter(hash_string name) const
{
    if ( name == hash_string("opacity") ) {
        return opacityBinder.get();
    }

    return base_type::getParameter(name);
}

bool LightingEffect::bindParameter(hash_string                        name,
                                   const abstract_parameter_binding*    binding)
{
    if ( name == hash_string("opacity") && ((opacityBinder = cast_binding<float>(binding)) || !binding) )
    {
        isDirty = true;
        return true;
    }
    else if ( base_type::bindParameter(name, binding) ) {
        return true;
    }

    return false;
}

int LightingEffect::queryAttribute(hash_string name)
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
                detail::Pass::UNIFORM_DESC uniformDesc[14];
                {
                    uniformDesc[0].uniformName   = "projectionMatrix";
                    uniformDesc[0].parameterName = "projectionMatrix";
                    uniformDesc[1].uniformName   = "lightColorIntensity";
                    uniformDesc[1].parameterName = "lightColorIntensity";
                    uniformDesc[2].uniformName   = "lightDirectionAmbient";
                    uniformDesc[2].parameterName = "lightViewDirectionAmbient";
                    uniformDesc[3].uniformName   = "lightPositionRadius";
                    uniformDesc[3].parameterName = "lightViewPositionRadius";
                    uniformDesc[4].uniformName   = "materialDiffuseSpecular";
                    uniformDesc[4].parameter     = material->diffuseSpecularBinder.get();
                    uniformDesc[5].uniformName   = "materialShininess";
                    uniformDesc[5].parameter     = material->shininessBinder.get();
                    uniformDesc[6].uniformName   = "normalMatrix";
                    uniformDesc[6].parameter     = normalMatrixBinder.get();
                    uniformDesc[7].uniformName   = "worldViewMatrix";
                    uniformDesc[7].parameter     = worldViewMatrixBinder.get();
                    uniformDesc[8].uniformName   = "worldViewProjMatrix";
                    uniformDesc[8].parameter     = worldViewProjMatrixBinder.get();
                    uniformDesc[9].uniformName   = "diffuseSpecularMap";
                    uniformDesc[9].parameter     = material->diffuseSpecularMapBinder.get();
                    uniformDesc[10].uniformName  = "opacity";
                    uniformDesc[10].parameter    = opacityBinder ? opacityBinder.get() : opacityOneBinder.get();

					if (boneMatricesBinder) 
					{
						uniformDesc[11].uniformName = "boneMatrices";
						uniformDesc[11].parameter   = boneMatricesBinder.get();
					}

                    if (boneRotationsBinder)
                    {
				        uniformDesc[12].uniformName = "boneRotations";
				        uniformDesc[12].parameter   = boneRotationsBinder.get();
                    }

                    if (boneTranslationsBinder)
                    {
				        uniformDesc[13].uniformName = "boneTranslations";
				        uniformDesc[13].parameter   = boneTranslationsBinder.get();
                    }

					desc.uniforms    = uniformDesc;
					desc.numUniforms = 14;
                }

                // create passes
                EffectShaderProgram baseProgram(AUTO_LOGGER);
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
            AUTO_LOGGER_MESSAGE(log::S_ERROR, "Deferred renderer is not supported by lighting effect." << std::endl);
            break;
        }
    }
    catch(slon_error&)
    {
        AUTO_LOGGER_MESSAGE(log::S_ERROR, "Can't create lighting effect." << std::endl);
    }

	TransformEffect::dirty();
}

} // namesapce slon
} // namespace graphics
