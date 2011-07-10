#include "stdafx.h"
#include "Graphics/PostProcess/Common.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Effect.h"
#include "Graphics/GraphicsManager.h"
#include "Graphics/FogFilter.h"
#include "Graphics/Common.h"
#include "Log/Logger.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("graphics.FogFilter");

namespace slon {
namespace graphics {

FogShaderProgram::FogShaderProgram(bool useAlphaBlend) :
    EffectShaderProgram(AUTO_LOGGER)
{
    addShader("Data/Shaders/PostProcess/fog.vert");
    addShader("Data/Shaders/PostProcess/fog.frag");
    addShader("Data/Shaders/depth.frag");
    if (useAlphaBlend) {
        addDefinition("#define USE_ALPHA_BLEND");
    }
    constructProgram();
}

void FogShaderProgram::redirectUniforms()
{
    // get uniforms
    inputMapUniform = loadSamplerUniform<sgl::Texture2D>("inputMap");
    depthMapUniform = loadSamplerUniform<sgl::Texture2D>("depthMap");

    projectionMatrixInverseUniform  = loadUniform<math::Matrix4f>("projectionMatrixInverse");
    depthParamsUniform              = loadUniform<math::Vector4f>("depthParams");
    directionUniform                = loadUniform<math::Vector3f>("fogDirection");
    colorUniform                    = loadUniform<math::Vector3f>("fogColor");
    densityUniform                  = loadUniform<float>("fogDensity");
    levelUniform                    = loadUniform<float>("fogLevel");
    heightFalloffUniform            = loadUniform<float>("fogHeightFalloff");
    frustumFarUniform               = loadUniform<float>("frustumFar");
}

FogFilter::FogFilter() :
    fogProgram(false),
    fogBlendProgram(true),
    fogColor(1.0f, 1.0f, 1.0f, 1.0f),
    fogLevel(0.0f),
    fogDensity(0.001f),
    fogHeightFalloff(0.01f)
{
    // require params
    detail::ParameterTable& parameterTable = detail::currentParameterTable();

    inputMapBinder                 = parameterTable.getParameterBinding<sgl::Texture2D>( hash_string("inputMap") );
    depthMapBinder                 = parameterTable.getParameterBinding<sgl::Texture2D>( hash_string("depthMap") );
    projectionMatrixInverseBinder  = parameterTable.getParameterBinding<math::Matrix4f>( hash_string("invProjectionMatrix") );
    normalMatrixBinder             = parameterTable.getParameterBinding<math::Matrix3f>( hash_string("normalMatrix") );

    assert(inputMapBinder);
    assert(depthMapBinder);
    assert(projectionMatrixInverseBinder);
    assert(normalMatrixBinder);
}

// Override Filter
unsigned FogFilter::perform( sgl::RenderTarget*     renderTarget,
                             unsigned               source ) const
{
    int          numSwitches = 0;
    sgl::Device* device      = currentDevice();
    if (!renderTarget)
    {
        ppu::DrawUtilities::beginChain();
        {
            fogProgram.getProgram()->Bind();

            // sampler uniforms
            fogProgram.inputMapUniform->Set( 0, inputMapBinder->values() );
            fogProgram.depthMapUniform->Set( 1, depthMapBinder->values() );

            // depth uniforms
            const math::Matrix4f& projectionMatrixInverse = projectionMatrixInverseBinder->value();
            fogProgram.projectionMatrixInverseUniform->Set(projectionMatrixInverse);
            fogProgram.depthParamsUniform->Set( math::Vector4f(projectionMatrixInverse[2][2],
                                                               projectionMatrixInverse[2][3],
                                                               projectionMatrixInverse[3][2],
                                                               projectionMatrixInverse[3][3]) );

            fogProgram.colorUniform->Set( math::xyz(fogColor) );
            fogProgram.densityUniform->Set(fogDensity);
            fogProgram.levelUniform->Set(fogLevel);
            fogProgram.directionUniform->Set( normalMatrixBinder->value() * math::Vector3f(0.0f, 1.0f, 0.0f) );
            fogProgram.heightFalloffUniform->Set(fogHeightFalloff);
            device->Draw(sgl::QUADS, 0 ,4);
        }
        ppu::DrawUtilities::endChain();

        numSwitches = 1;
    }
    else
    {
        sgl::Texture2D* attachments[2] =
        {
            static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(0) ),
            static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(1) )
        };

        assert(attachments[0] && attachments[1]);
    }

    return numSwitches;
}

} // namespace graphics
} // namespace slon
