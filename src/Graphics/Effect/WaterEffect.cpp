#include "stdafx.h"
#include "Engine.h"
#include "Graphics/Effect/PhillipsSpectrum.h"
#include "Graphics/Effect/WaterEffect.h"
#include "Graphics/Effect/Detail/EffectShaderProgram.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/Common.h"
#include "Graphics/GraphicsManager.h"
#include "Graphics/PostProcess/Common.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Log/Logger.h"
#include "Scene/Light/DirectionalLight.h"
#include <sgl/Math/MatrixFunctions.hpp>

//#define DEBUG_WATER_PERFORMANCE

__DEFINE_LOGGER__("graphics.WaterEffect")

namespace slon {
namespace graphics {

    /* Program for generating frequencies using spectrum. */
    class FrequenciesProgram :
        public EffectShaderProgram
    {
    public:
        inline FrequenciesProgram() :
            EffectShaderProgram(logger)
        {
            addShader("Data/Shaders/Water/Frequencies.vert");
            addShader("Data/Shaders/Water/Frequencies.frag");
            constructProgram();
        }

    protected:
        void redirectUniforms()
        {
        	using namespace sgl;
            using namespace math;

            frequenciesMapUniform   = loadSamplerUniform<Texture2D>("frequenciesMap");
            gridSizeUniform         = loadUniform<Vector2f>("gridSize");
            surfaceSizeUniform      = loadUniform<Vector2f>("surfaceSize");
            timeUniform             = loadUniform<float>("time");
        }

    public:
        // uniforms
        sgl::SamplerUniform2D*      frequenciesMapUniform;
        sgl::Uniform2F*             gridSizeUniform;
        sgl::Uniform2F*             surfaceSizeUniform;
        sgl::UniformF*              timeUniform;
    };

    /* Program for generating additional water maps. */
    class MapsProgram :
        public EffectShaderProgram
    {
    public:
        inline MapsProgram() :
            EffectShaderProgram(logger)
        {
            addShader("Data/Shaders/Water/Transform.vert");
            addShader("Data/Shaders/Water/OceanMaps.frag");
            constructProgram();
        }

    protected:
        void redirectUniforms()
        {
        	using namespace sgl;
            using namespace math;

            fftMapUniform           = loadSamplerUniform<Texture2D>("fftMap");
            gridSizeUniform         = loadUniform<Vector2f>("gridSize");
            surfaceSizeUniform      = loadUniform<Vector2f>("surfaceSize");
            sharpnessUniform        = loadUniform<float>("sharpness");
            amplitudeUniform        = loadUniform<float>("amplitude");
        }

    public:
        // uniforms
        sgl::SamplerUniform2D*      fftMapUniform;
        sgl::Uniform2F*             gridSizeUniform;
        sgl::Uniform2F*             surfaceSizeUniform;
        sgl::UniformF*              sharpnessUniform;
        sgl::UniformF*              amplitudeUniform;
    };

    /* Generates maps for FFT */
    class FFTMapsGenerator :
        public Referenced
    {
    public:
        FFTMapsGenerator() :
            lastFrameUpdate(0)
        {
            mapsRenderTarget.reset( currentDevice()->CreateRenderTarget() );
        }

        void set_frequency_spectrum(const boost::shared_ptr<FrequencySpectrum>& _frequencySpectrum)
        {
            if (frequencySpectrum == _frequencySpectrum) {
                return;
            }

            using namespace sgl;
            using namespace math;
            using namespace slon::thread;

            // Otherwise perform function
            Device*       device = currentDevice();
            DeviceTraits* traits = device->Traits();

            frequencySpectrum = _frequencySpectrum;
            int   N           = frequencySpectrum->getGridSize();
            float size        = static_cast<float>(N);

            // create fft filter
            fftFilter.setDirection(-1);
            fftFilter.setIndexShift( Vector2i(-N/2, -N/2) );

            // create fft render target
            Texture2D* fftTexture[2];
            {
                Texture2D::DESC desc;
                desc.width  = N;
                desc.height = N;
                desc.format = Texture::RGBA32F;

                for (int i = 0; i<2; ++i)
                {
                    fftTexture[i] = device->CreateTexture2D(desc);
                    if (!fftTexture[i]) {
                        throw gl_error(logger, "Can't create texture for performing fft.");
                    }

                    sgl::SamplerState::DESC ssDesc;
                    ssDesc.filter[0]   = (traits->ShaderModel() >= 400) ? SamplerState::LINEAR : SamplerState::NEAREST;
                    ssDesc.filter[1]   = (traits->ShaderModel() >= 400) ? SamplerState::LINEAR : SamplerState::NEAREST;
                    ssDesc.filter[2]   = SamplerState::NONE;

                    ssDesc.wrapping[0] = sgl::SamplerState::REPEAT;
                    ssDesc.wrapping[1] = sgl::SamplerState::REPEAT;
                    ssDesc.wrapping[2] = sgl::SamplerState::REPEAT;

                    fftTexture[i]->BindSamplerState( device->CreateSamplerState(ssDesc) );
                }
            }
            heightMap.reset(fftTexture[0]);

            fftRenderTarget.reset( device->CreateRenderTarget() );
            fftRenderTarget->SetColorAttachment(0, fftTexture[0], 0);
            fftRenderTarget->SetColorAttachment(1, fftTexture[1], 0);
            if ( SGL_OK != fftRenderTarget->Dirty() ) {
                throw gl_error(logger, "Can't create render taget for rendering fft.");
            }

            // create ocean normal map
            {
                sgl::Texture2D::DESC desc;
                desc.format = Texture::RGB8;
                desc.width  = N;
                desc.height = N;
                desc.data   = 0;
                normalMap.reset( device->CreateTexture2D(desc) );
                normalMap->GenerateMipmap();

                sgl::SamplerState::DESC ssDesc;
                ssDesc.filter[0] = SamplerState::LINEAR;
                ssDesc.filter[1] = SamplerState::LINEAR;
                ssDesc.filter[2] = SamplerState::LINEAR;
                normalMap->BindSamplerState( device->CreateSamplerState(ssDesc) );
            }

            mapsRenderTarget->SetColorAttachment(0, normalMap.get(), 0);
            mapsRenderTarget->SetDrawBuffer(0);
            if ( sgl::SGL_OK != mapsRenderTarget->Dirty() ) {
                throw gl_error(logger, "Can't create render taget for rendering ocean normal map.");
            }

            // set uniform values
            frequenciesProgram.getProgram()->Bind();
            frequenciesProgram.gridSizeUniform->Set( Vector2f(size, size) );
            frequenciesProgram.surfaceSizeUniform->Set( frequencySpectrum->getSurfaceSize() );

            mapsProgram.getProgram()->Bind();
            mapsProgram.gridSizeUniform->Set( Vector2f(size, size) );
            mapsProgram.surfaceSizeUniform->Set( frequencySpectrum->getSurfaceSize() );
            mapsProgram.getProgram()->Unbind();
        }

        void generate_maps()
        {
            // generate maps once per frame
            Engine* engine = Engine::Instance();
            if ( lastFrameUpdate == engine->getFrameNumber() ) {
                return;
            }
            lastFrameUpdate = engine->getFrameNumber();

            float time = float( engine->getSimulationTimer().getTime() );

            // remembder render target
            sgl::Device*                device = currentDevice();
            const sgl::RenderTarget*    oldRT  = device->CurrentRenderTarget();

            fftRenderTarget->SetDrawBuffer(0);
            if ( sgl::SGL_OK == fftRenderTarget->Bind() )
            {
                int N = heightMap->Width();

                ppu::DrawUtilities::beginChain();
                device->SetViewport( sgl::rectangle(0, 0, N, N) );

                // generate coefficients
                frequenciesProgram.getProgram()->Bind();
                frequenciesProgram.timeUniform->Set(time);
                frequenciesProgram.frequenciesMapUniform->Set( 0, frequencySpectrum->getFrequenciesMap() );
                device->Draw(sgl::QUADS, 0, 4);

                // perform FFT
                fftFilter.perform(fftRenderTarget.get(), 0);

                // generate normal map
                if ( sgl::SGL_OK == mapsRenderTarget->Bind() )
                {
                    mapsProgram.getProgram()->Bind();
                    mapsProgram.fftMapUniform->Set( 0, heightMap.get() );
                    device->Draw(sgl::QUADS, 0, 4);
                    mapsRenderTarget->Unbind();
                    normalMap->GenerateMipmap();
                }

                ppu::DrawUtilities::endChain();
            }

            if (oldRT) {
                oldRT->Bind();
            }
        }

    public:
        // generator
        boost::shared_ptr<FrequencySpectrum>    frequencySpectrum;
        FFTFilter                               fftFilter;
        sgl::ref_ptr<sgl::RenderTarget>         fftRenderTarget;
        sgl::ref_ptr<sgl::RenderTarget>         mapsRenderTarget;
        mutable int                             lastFrameUpdate;

        // programs
        FrequenciesProgram  frequenciesProgram;
        MapsProgram         mapsProgram;

        // maps
        sgl::ref_ptr<sgl::Texture2D>    heightMap;
        sgl::ref_ptr<sgl::Texture2D>    normalMap;
    };

    typedef boost::intrusive_ptr<FFTMapsGenerator> fft_maps_generator_ptr;

WaterEffect::WaterEffect(const frequency_spectrum_ptr& _frequencySpectrum) :
    squeezing(5.0f),
    waveSharpness(0.5f),
    waterTransparency(0.5f),
    distanceSmoothness(0.005f),
    frequencySpectrum(_frequencySpectrum)
{
    surfaceSize = frequencySpectrum->getSurfaceSize();
    dirtyShaderTechniques();

    // create maps generator
    mapsGenerator.reset(new FFTMapsGenerator);

    // get binders
    {
        detail::ParameterTable& parameterTable  = detail::currentParameterTable();
        viewMatrixBinder                        = parameterTable.getParameterBinding<math::Matrix4f>( unique_string("viewMatrix") );
        viewMatrixInverseBinder                 = parameterTable.getParameterBinding<math::Matrix4f>( unique_string("invViewMatrix") );
    }

    // create binders
    worldViewMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&worldViewMatrix, 1, false) );
    reflectionMatrixBinder.reset( new parameter_binding<math::Matrix4f>(&reflectionMatrix, 1, false) );
    squeezeParamsBinder.reset( new parameter_binding<math::Vector2f>(&squeezeParams, 1, false) );
    surfaceSizeBinder.reset( new parameter_binding<math::Vector2f>(&surfaceSize, 1, false) );
    sharpnessBinder.reset( new parameter_binding<float>(&waveSharpness, 1, false) );
    distanceSmoothnessBinder.reset( new parameter_binding<float>(&distanceSmoothness, 1, false) );
    waterTransparencyBinder.reset( new parameter_binding<float>(&waterTransparency, 1, false) );
    heightMapBinder.reset( new parameter_binding<sgl::Texture2D>(mapsGenerator->heightMap.get(), 1, false) );
    normalMapBinder.reset( new parameter_binding<sgl::Texture2D>(mapsGenerator->normalMap.get(), 1, false) );
    reflectMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
    refractMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
    depthMapBinder.reset( new parameter_binding<sgl::Texture2D>(0, 1, false) );
    environmentMapBinder.reset( new parameter_binding<sgl::TextureCube>(0, 1, false) );
}

// Lighting effect
void WaterEffect::dirtyShaderTechniques()
{
    std::for_each( passes,
                   passes + scene::Light::NUM_LIGHT_TYPES,
                   boost::bind(&pass_ptr::reset, _1, (Pass*)0) );

    // create technique accordingly to renderer
    Renderer::RENDER_TECHNIQUE technique = currentGraphicsManager().getRenderer()->getRenderTechnique();
    try
    {
        switch (technique)
        {
            case Renderer::FORWARD_RENDERING:
            {
                using detail::ForwardRenderer;

                detail::Pass::DESC         desc;
                detail::Pass::UNIFORM_DESC uniformDesc[20];
                {
                    uniformDesc[0].uniformName   = "projectionMatrix";
                    uniformDesc[0].parameterName = "projectionMatrix";
                    uniformDesc[1].uniformName   = "depthParams";
                    uniformDesc[1].parameterName = "depthParams";
                    uniformDesc[2].uniformName   = "eyePosition";
                    uniformDesc[2].parameterName = "eyePosition";
                    uniformDesc[3].uniformName   = "lightColorIntensity";
                    uniformDesc[3].parameterName = "lightColorIntensity";
                    uniformDesc[4].uniformName   = "lightDirectionAmbient";
                    uniformDesc[4].parameterName = "lightDirectionAmbient";
                    uniformDesc[5].uniformName   = "depthParams";
                    uniformDesc[5].parameterName = "depthParams";
                    uniformDesc[6].uniformName   = "sharpness";
                    uniformDesc[6].parameter     = sharpnessBinder.get();
                    uniformDesc[7].uniformName   = "distanceSmoothness";
                    uniformDesc[7].parameter     = distanceSmoothnessBinder.get();
                    uniformDesc[8].uniformName   = "waterTransparency";
                    uniformDesc[8].parameter     = waterTransparencyBinder.get();
                    uniformDesc[9].uniformName   = "squeezeParams";
                    uniformDesc[9].parameter     = squeezeParamsBinder.get();
                    uniformDesc[10].uniformName  = "surfaceSize";
                    uniformDesc[10].parameter    = surfaceSizeBinder.get();
                    uniformDesc[11].uniformName  = "surfaceCorners";
                    uniformDesc[11].parameter    = projectedGridCornersBinder.get();
                    uniformDesc[12].uniformName  = "worldViewMatrix";
                    uniformDesc[12].parameter    = worldViewMatrixBinder.get();
                    uniformDesc[13].uniformName  = "reflectionMatrix";
                    uniformDesc[13].parameter    = reflectionMatrixBinder.get();
                    uniformDesc[14].uniformName  = "heightMap";
                    uniformDesc[14].parameter    = heightMapBinder.get();
                    uniformDesc[15].uniformName  = "normalMap";
                    uniformDesc[15].parameter    = normalMapBinder.get();
                    uniformDesc[16].uniformName  = "reflectMap";
                    uniformDesc[16].parameter    = reflectMapBinder.get();
                    uniformDesc[17].uniformName  = "refractMap";
                    uniformDesc[17].parameter    = refractMapBinder.get();
                    uniformDesc[18].uniformName  = "depthMap";
                    uniformDesc[18].parameter    = depthMapBinder.get();
                    uniformDesc[19].uniformName  = "environmentMap";
                    uniformDesc[19].parameter    = environmentMapBinder.get();

					desc.uniforms    = uniformDesc;
					desc.numUniforms = 20;
                }

                // create passes
                EffectShaderProgram baseProgram(logger);
                baseProgram.addShader("Data/Shaders/Water/Ocean.vert");
                baseProgram.addShader("Data/Shaders/Water/Ocean.frag");
                baseProgram.addShader("Data/Shaders/depth.frag");
                if (reflectTexture) {
                    baseProgram.addDefinition("#define ENABLE_REFLECTIONS");
                }
                if (refractTexture) {
                    baseProgram.addDefinition("#define ENABLE_REFRACTIONS");
                }
                if (depthTexture) {
                    baseProgram.addDefinition("#define ENABLE_DEPTH_MAP");
                }

                EffectShaderProgram directionalProgram(baseProgram);
                directionalProgram.addDefinition("#define DIRECTIONAL_LIGHTING");
                desc.program = directionalProgram.getProgram();
                desc.priority = ForwardRenderer::makePriority(ForwardRenderer::OPAQUE_BIN, desc.program);
                passes[scene::Light::DIRECTIONAL].reset( new detail::Pass(desc) );
/*
                program = baseProgram;
                program.addDefinition("#define POINT_LIGHTING");
                desc.program = program.getProgram();
                passes[scene::Light::POINT].reset( new detail::Pass(desc) );
*/
                break; 
            }

        case Renderer::FIXED_PIPELINE:
            logger << log::S_ERROR << "Fixed pipeline renderer is not supported by water effect." << std::endl;
            break;

        case Renderer::DEFERRED_SHADING:
            logger << log::S_ERROR << "Deferred renderer is not supported by water effect." << std::endl;
            break;
        }
    }
    catch(slon_error&)
    {
        logger << log::S_ERROR << "Can't create lighting effect." << std::endl;
    }
}

void WaterEffect::setFrequencySpectrum(const frequency_spectrum_ptr& _frequencySpectrum)
{
    bool dirtyShaders = (frequencySpectrum == 0) && (_frequencySpectrum != 0);
    frequencySpectrum = _frequencySpectrum;
    mapsGenerator->set_frequency_spectrum(frequencySpectrum);
    surfaceSizeBinder->write_value( frequencySpectrum->getSurfaceSize() );
    if (dirtyShaders) {
        dirtyShaderTechniques();
    }
}

void WaterEffect::setWaveSharpness(float waveSharpness) 
{ 
    sharpnessBinder->write_value(waveSharpness);
}

void WaterEffect::setWaterTransparency(float waterTransparency) 
{
    waterTransparencyBinder->write_value(waterTransparency); 
}

void WaterEffect::setDistanceSmoothness(float distanceSmoothness) 
{ 
    distanceSmoothnessBinder->write_value(distanceSmoothness);
}

void WaterEffect::setEnvironmentTexture(sgl::TextureCube* _environmentTexture)
{
    environmentTexture.reset(_environmentTexture); 
    environmentMapBinder->switch_values(_environmentTexture, 1, false);
}

void WaterEffect::setReflectCameraAndTexture(const scene::Camera* _reflectCamera, sgl::Texture2D* _reflectTexture)
{
    assert( (_reflectCamera && _reflectTexture) || (!_reflectCamera && !_reflectTexture) );

    reflectCamera.reset(_reflectCamera);
    reflectTexture.reset(_reflectTexture);
    reflectMapBinder->switch_values(_reflectTexture, 1, false);
    dirtyShaderTechniques();
}

void WaterEffect::setRefractTexture(sgl::Texture2D* _refractTexture)
{
    refractTexture.reset(_refractTexture);
    refractMapBinder->switch_values(_refractTexture, 1, false);
    dirtyShaderTechniques();
}

void WaterEffect::setDepthTexture(sgl::Texture2D* _depthTexture)
{
    depthTexture.reset(_depthTexture);
    depthMapBinder->switch_values(_depthTexture, 1, false);
    dirtyShaderTechniques();
}

int WaterEffect::present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** p)
{
    if ( renderGroup == detail::ForwardRenderer::mainGroupHandle()
         && renderPass == detail::ForwardRenderer::opaquePassHandle() ) 
    {
        // setup spectrum if changed
        mapsGenerator->set_frequency_spectrum(frequencySpectrum);
        mapsGenerator->generate_maps();

        heightMapBinder->switch_values(mapsGenerator->heightMap.get(), 1, false);
        normalMapBinder->switch_values(mapsGenerator->normalMap.get(), 1, false);

        // get device
        sgl::Device* device = currentDevice();

        // copy textures from fbo
        if (refractTexture)
        {
            device->CopyTexture2D( refractTexture.get(), 0, 0, 0, refractTexture->Width(), refractTexture->Height() );
            refractTexture->GenerateMipmap();
        }
        if (depthTexture) {
            device->CopyTexture2D( depthTexture.get(), 0, 0, 0, depthTexture->Width(), depthTexture->Height() );
        }

        if (reflectTexture)
        {
            reflectionMatrixBinder->write_value( math::make_translation(0.5f, 0.5f, 0.0f)
                                                 * math::make_scaling(0.5f, 0.5f, 1.0f)
                                                 * reflectCamera->getProjectionMatrix()
                                                 * reflectCamera->getViewMatrix() );
        }

        // calculate squeezing with respect to camera direction
        {
            const math::Matrix4f& view  = viewMatrixBinder->value();
            math::Vector3f        dir   = math::normalize( math::Vector3f(view[2][0], view[2][1], view[2][2]) );

            float   squeezingFactor  = sqrtf(1.0f - fabs(dir.y));
            float   squeezingAdapted = 1.0f + (squeezing - 1.0f) * squeezingFactor;

            squeezeParamsBinder->write_value( math::Vector2f(1.0f / ( exp(squeezingAdapted) - 1.0f ), squeezingAdapted) );
        }

        worldViewMatrixBinder->write_value( viewMatrixBinder->value() );

        p[0] = passes[scene::Light::DIRECTIONAL].get();
        return 1;
    }

    return 0;
}

const abstract_parameter_binding* WaterEffect::getParameter(unique_string name) const
{
    if ( name == unique_string("projectedGridCorners") ) {
        return projectedGridCornersBinder.get();
    }
    else if ( name == unique_string("allowCulling") ) {
        return allowCullingBinder.get();
    }

    return 0;
}

bool WaterEffect::bindParameter(unique_string                     name,
                                const abstract_parameter_binding* binding)
{
    if (name == unique_string("projectedGridCorners") && (projectedGridCornersBinder = cast_binding<math::Vector3f>(binding)) ) 
    {
        dirtyShaderTechniques();
        return true;
    }
    else if (name == unique_string("allowCulling") && (allowCullingBinder = cast_binding<bool>(binding)) ) {
        return true;
    }

    return false;
}

} // namesapce slon
} // namespace graphics
