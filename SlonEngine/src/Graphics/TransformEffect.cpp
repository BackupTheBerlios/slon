#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/TransformEffect.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Detail/Pass.h"
#include "Graphics/ForwardRenderer.h"

DECLARE_AUTO_LOGGER("graphics.TransformEffect")

namespace slon {
namespace graphics {

TransformEffect::TransformEffect()
:	isDirty(true)
{
    // query parameters
    detail::ParameterTable& parameterTable = detail::currentParameterTable();
    viewMatrixBinder                       = parameterTable.getParameterBinding<math::Matrix4f>("viewMatrix");
    projectionMatrixBinder                 = parameterTable.getParameterBinding<math::Matrix4f>("projectionMatrix");

    // create binders
    worldViewMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&worldViewMatrix, 1, false) );
    worldViewProjMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&worldViewProjMatrix, 1, false) );
    normalMatrixBinder.reset( new parameter_binding<math::Matrix3f>(&normalMatrix, 1, false) );
}

void TransformEffect::dirty()
{
    // create pass
    if ( isDirty && currentRenderer()->getRenderTechnique() != Renderer::FIXED_PIPELINE ) 
    {
        EffectShaderProgram program(AUTO_LOGGER);
		if (boneMatricesBinder || (boneRotationsBinder && boneTranslationsBinder)) {
			program.addShader("Data/Shaders/skinned.vert");
		}
		else {
			program.addShader("Data/Shaders/rigid.vert");
		}
        program.addShader("Data/Shaders/fill.frag");
        program.addDefinition("#define DEPTH_ONLY");

        detail::Pass::DESC desc;
        desc.program = program.getProgram();

        detail::Pass::UNIFORM_DESC uniformDesc[4];
		{
			uniformDesc[0].uniformName = "worldViewProjMatrix";
			uniformDesc[0].parameter   = worldViewProjMatrixBinder.get();

			if (boneMatricesBinder) 
			{
				uniformDesc[1].uniformName = "boneMatrices";
				uniformDesc[1].parameter   = boneMatricesBinder.get();
			}

            if (boneRotationsBinder)
            {
				uniformDesc[2].uniformName = "boneRotations";
				uniformDesc[2].parameter   = boneRotationsBinder.get();
            }

            if (boneTranslationsBinder)
            {
				uniformDesc[3].uniformName = "boneTranslations";
				uniformDesc[3].parameter   = boneTranslationsBinder.get();
            }

			desc.uniforms    = uniformDesc;
			desc.numUniforms = 4;
		}
        
        sgl::DepthStencilState::DESC dsDesc;
        {
            dsDesc.depthEnable    = true;
            dsDesc.depthFunc      = sgl::DepthStencilState::LEQUAL;
            dsDesc.stencilEnable  = false;
            dsDesc.depthWriteMask = true;
        }
        desc.depthStencilState = currentDevice()->CreateDepthStencilState(dsDesc);
        
        sgl::RasterizerState::DESC rastDesc;
        {
            rastDesc.cullMode  = sgl::RasterizerState::BACK;
            rastDesc.fillMode  = sgl::RasterizerState::SOLID;
            rastDesc.colorMask = 0;
        }
        desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);

        depthPass.reset( new detail::Pass(desc) );

        // create back faces pass
        {
            rastDesc.cullMode  = sgl::RasterizerState::FRONT;
            rastDesc.fillMode  = sgl::RasterizerState::SOLID;
            rastDesc.colorMask = 0;
        }
        desc.rasterizerState = currentDevice()->CreateRasterizerState(rastDesc);
        
        backFaceDepthPass.reset( new detail::Pass(desc) );
		
        isDirty = false;
    }
}

// Override Effect
int TransformEffect::present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes)
{
	if (isDirty) {
		dirty();
	}

    if ( renderPass == detail::ForwardRenderer::RP_DEPTH)
    {
        // prepare values for uniforms
        worldViewProjMatrixBinder->write_value( projectionMatrixBinder->value() * viewMatrixBinder->value() * worldMatrixBinder->value() );

        // get pass
        if ( renderGroup == detail::ForwardRenderer::RG_REFLECT ) {
            passes[0] = backFaceDepthPass.get();
        }
        else {
            passes[0] = depthPass.get();
        }
        return 1;
    }

    return 0;
}

const abstract_parameter_binding* TransformEffect::getParameter(hash_string name) const
{
    if ( name == hash_string("worldMatrix") ) {
        return worldMatrixBinder.get();
    }
    else if ( name == hash_string("boneRotations") ) {
        return boneRotationsBinder.get();
    }
    else if ( name == hash_string("boneTranslations") ) {
        return boneTranslationsBinder.get();
    }

    return 0;
}

bool TransformEffect::bindParameter(hash_string                        name,
                                    const abstract_parameter_binding*    binding)
{
    if ( name == hash_string("worldMatrix") && (worldMatrixBinder = cast_binding<math::Matrix4f>(binding)) ) {
        return true;
    }

	if ( currentRenderer()->getRenderTechnique() != Renderer::FIXED_PIPELINE )
	{
        /*
		if ( name == hash_string("boneMatrices") && (boneMatricesBinder = cast_binding<math::Matrix4f>(binding)) ) 
		{
			isDirty = true;
			return true;
		}
        */
		if ( name == hash_string("boneRotations") && (boneRotationsBinder = cast_binding<math::Vector4f>(binding)) ) 
		{
			isDirty = true;
			return true;
		}
        else if ( name == hash_string("boneTranslations") && (boneTranslationsBinder = cast_binding<math::Vector3f>(binding)) ) 
        {
			isDirty = true;
			return true;
        }
	}

    return false;
}

int TransformEffect::queryAttribute(hash_string name)
{
    return detail::currentAttributeTable().queryAttribute(name)->index;
}

} // namesapce slon
} // namespace graphics
