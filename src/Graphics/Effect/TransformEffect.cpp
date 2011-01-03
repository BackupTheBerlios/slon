#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Effect/TransformEffect.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Renderer/ForwardRenderer.h"

__DEFINE_LOGGER__("graphics.TransformEffect")

namespace slon {
namespace graphics {

TransformEffect::TransformEffect()
:	isDirty(true)
{
    // query parameters
    detail::ParameterTable& parameterTable = detail::currentParameterTable();
    viewMatrixBinder                       = parameterTable.getParameterBinding<math::Matrix4f>( unique_string("viewMatrix") );
    projectionMatrixBinder                 = parameterTable.getParameterBinding<math::Matrix4f>( unique_string("projectionMatrix") );

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
        EffectShaderProgram program(logger);
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

        detail::Pass::UNIFORM_DESC uniformDesc;
		{
			uniformDesc.uniformName = "worldViewProjMatrix";
			uniformDesc.parameter   = worldViewProjMatrixBinder.get();
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

    if ( renderPass == detail::ForwardRenderer::depthPassHandle() )
    {
        // prepare values for uniforms
        worldViewProjMatrixBinder->write_value( projectionMatrixBinder->value() * viewMatrixBinder->value() * worldMatrixBinder->value() );

        // get pass
        if ( renderGroup == detail::ForwardRenderer::reflectGroupHandle() ) {
            passes[0] = backFaceDepthPass.get();
        }
        else {
            passes[0] = depthPass.get();
        }
        return 1;
    }

    return 0;
}

const abstract_parameter_binding* TransformEffect::getParameter(unique_string name) const
{
    if ( name == unique_string("worldMatrix") ) {
        return worldMatrixBinder.get();
    }
    else if ( name == unique_string("boneRotations") ) {
        return boneRotationsBinder.get();
    }
    else if ( name == unique_string("boneTranslations") ) {
        return boneTranslationsBinder.get();
    }

    return 0;
}

bool TransformEffect::bindParameter(unique_string                        name,
                                    const abstract_parameter_binding*    binding)
{
    if ( name == unique_string("worldMatrix") && (worldMatrixBinder = cast_binding<math::Matrix4f>(binding)) ) {
        return true;
    }

	if ( currentRenderer()->getRenderTechnique() != Renderer::FIXED_PIPELINE )
	{
        /*
		if ( name == unique_string("boneMatrices") && (boneMatricesBinder = cast_binding<math::Matrix4f>(binding)) ) 
		{
			isDirty = true;
			return true;
		}
        */
		if ( name == unique_string("boneRotations") && (boneRotationsBinder = cast_binding<math::Vector4f>(binding)) ) 
		{
			isDirty = true;
			return true;
		}
        else if ( name == unique_string("boneTranslations") && (boneTranslationsBinder = cast_binding<math::Vector3f>(binding)) ) 
        {
			isDirty = true;
			return true;
        }
	}

    return false;
}

int TransformEffect::queryAttribute(unique_string name)
{
    return detail::currentAttributeTable().queryAttribute(name)->index;
}

} // namesapce slon
} // namespace graphics
