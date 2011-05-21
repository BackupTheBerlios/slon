#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0 // disable debug new due to conflict with sgl::Aligned allocator
#include "Graphics/Common.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Renderable.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Log/Logger.h"
#include "Scene/Camera/ReflectCamera.h"
#include "Scene/Light/DirectionalLight.h"
#include "Scene/Light/PointLight.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("graphics.ForwardRenderer")

namespace {

    using namespace slon;
    using namespace slon::graphics;

    sgl::RenderTarget* updatePPURenderTarget( sgl::RenderTarget*    old,
                                              sgl::Texture::FORMAT  format,
                                              unsigned              width,
                                              unsigned              height,
                                              bool                  useDepthTexture )
    {
        sgl::Device*        device       = currentDevice();
        sgl::RenderTarget*  renderTarget = old;
        bool                dirty        = false;
        if (!renderTarget)
        {
            renderTarget = device->CreateRenderTarget();
            dirty        = true;
        }

        sgl::Texture2D* attachments[] =
        {
            static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(0) ),
            static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(1) )
        };

        for (int i = 0; i<2; ++i)
        {
            if (!attachments[i] || attachments[i]->Width() < width || attachments[i]->Height() < height)
            {
                sgl::Texture2D::DESC desc;
                desc.format = format;
                desc.width  = width;
                desc.height = height;
                desc.data   = 0;

                attachments[i] = device->CreateTexture2D(desc);
                if (!attachments[i]) {
                    throw gl_error(AUTO_LOGGER, "Can't create color attachment for post process render target.");
                }
                dirty = true;

                renderTarget->SetColorAttachment(i, attachments[i], 0);
            }
        }

        if (dirty)
        {
            sgl::Texture2D* depthTexture = 0;
            if (useDepthTexture)
            {
                sgl::Texture2D::DESC desc;
                desc.format = sgl::Texture::D24;
                desc.width  = width;
                desc.height = height;
                desc.data   = 0;

                depthTexture = device->CreateTexture2D(desc);
                if (!depthTexture) {
                    throw gl_error(AUTO_LOGGER, "Can't create depth texture for post process render target.");
                }

                // bind sampler state
                {
                    sgl::SamplerState::DESC desc;
                    desc.filter[0]   = sgl::SamplerState::NEAREST;
                    desc.filter[1]   = sgl::SamplerState::NEAREST;
                    desc.filter[2]   = sgl::SamplerState::NONE;

                    desc.wrapping[0] = sgl::SamplerState::CLAMP;
                    desc.wrapping[1] = sgl::SamplerState::CLAMP;

                    depthTexture->BindSamplerState( device->CreateSamplerState(desc) );
                }

                renderTarget->SetDepthStencilAttachment(depthTexture, 0);
            }
            else {
                renderTarget->SetDepthStencil(true, sgl::Texture::D24);
            }

            // create sampler state
            sgl::SamplerState::DESC desc;
            desc.filter[0]   = sgl::SamplerState::LINEAR;
            desc.filter[1]   = sgl::SamplerState::LINEAR;
            desc.filter[2]   = sgl::SamplerState::NONE;

            desc.wrapping[0] = sgl::SamplerState::CLAMP;
            desc.wrapping[1] = sgl::SamplerState::CLAMP;

            sgl::SamplerState* samplerState = device->CreateSamplerState(desc);
            attachments[0]->BindSamplerState(samplerState);
            attachments[1]->BindSamplerState(samplerState);

            if (sgl::SGL_OK != renderTarget->Dirty() ) {
                throw gl_error(AUTO_LOGGER, "Can't create render target for post processing.");
            }
        }

        return renderTarget;
    }

    struct partition_by_type
    {
        partition_by_type(scene::Light::LIGHT_TYPE _lightType) :
            lightType(_lightType)
        {}

        bool operator () (const scene::Light* light)
        {
            return light->getLightType() == lightType;
        }

        scene::Light::LIGHT_TYPE lightType;
    };

} // anonymous namespace

namespace slon {
namespace graphics {
namespace detail {

ForwardRenderer::camera_params::camera_params(detail::ParameterTable& parameterTable)
{
    normalMatrixBinder          = parameterTable.addParameterBinding( hash_string("normalMatrix"), &normalMatrix, 1, false);
    viewMatrixBinder            = parameterTable.addParameterBinding( hash_string("viewMatrix"), &viewMatrix, 1, false);
    invViewMatrixBinder         = parameterTable.addParameterBinding( hash_string("invViewMatrix"), &invViewMatrix, 1, false);
    projectionMatrixBinder      = parameterTable.addParameterBinding( hash_string("projectionMatrix"), &projectionMatrix, 1, false);
    invProjectionMatrixBinder   = parameterTable.addParameterBinding( hash_string("invProjectionMatrix"), &invProjectionMatrix, 1, false);
    depthParamsBinder           = parameterTable.addParameterBinding( hash_string("depthParams"), &depthParams, 1, false);
    eyePositionBinder           = parameterTable.addParameterBinding( hash_string("eyePosition"), &eyePosition, 1, false);
}

void ForwardRenderer::camera_params::setup(const scene::Camera& camera)
{
    normalMatrixBinder->write_value( camera.getNormalMatrix() );
    viewMatrixBinder->write_value( camera.getViewMatrix() );
    invViewMatrixBinder->write_value( camera.getInverseViewMatrix() );
    projectionMatrixBinder->write_value( camera.getProjectionMatrix() );
    invProjectionMatrixBinder->write_value( math::invert( camera.getProjectionMatrix() ) );
    depthParamsBinder->write_value( math::Vector4f(invProjectionMatrix[2][2], invProjectionMatrix[2][3], invProjectionMatrix[3][2], invProjectionMatrix[3][3]) );
    eyePositionBinder->write_value( math::Vector4f(invViewMatrix[0][3], invViewMatrix[1][3], invViewMatrix[2][3], invViewMatrix[3][3])  );
}

ForwardRenderer::light_params::light_params(detail::ParameterTable& parameterTable, unsigned maxLightCount_) :
    maxLightCount(maxLightCount_),
    lightViewDirectionAmbient(new math::Vector4f[maxLightCount]),
    lightViewPositionRadius(new math::Vector4f[maxLightCount]),
    lightDirectionAmbient(new math::Vector4f[maxLightCount]),
    lightPositionRadius(new math::Vector4f[maxLightCount]),
    lightColorIntensity(new math::Vector4f[maxLightCount])
{
    lightCountBinder                = parameterTable.addParameterBinding( hash_string("lightCount"), &lightCount, 1, false);
    lightViewDirectionAmbientBinder = parameterTable.addParameterBinding( hash_string("lightViewDirectionAmbient"),lightViewDirectionAmbient.get(), maxLightCount, false);
    lightViewPositionRadiusBinder   = parameterTable.addParameterBinding( hash_string("lightViewPositionRadius"), lightViewPositionRadius.get(), maxLightCount, false);
    lightDirectionAmbientBinder     = parameterTable.addParameterBinding( hash_string("lightDirectionAmbient"),lightDirectionAmbient.get(), maxLightCount, false);
    lightPositionRadiusBinder       = parameterTable.addParameterBinding( hash_string("lightPositionRadius"), lightPositionRadius.get(), maxLightCount, false);
    lightColorIntensityBinder       = parameterTable.addParameterBinding( hash_string("lightColorIntensity"), lightColorIntensity.get(), maxLightCount, false);
}

void ForwardRenderer::light_params::setup_directional(const scene::Camera& camera,
                                                      light_const_iterator firstLight, 
                                                      light_const_iterator endLight)
{
    lightCount = std::distance(firstLight, endLight);
    lightCountBinder->write_value(lightCount);
    for (size_t i = 0; firstLight != endLight; ++firstLight, ++i)
    {
        const scene::DirectionalLight* light = static_cast<const scene::DirectionalLight*>(*firstLight);
        lightDirectionAmbientBinder->write_value( math::make_vec( math::normalize(light->getDirection()), light->getAmbient() ), i );
        lightViewDirectionAmbientBinder->write_value( math::make_vec( math::normalize(camera.getNormalMatrix() * light->getDirection()), light->getAmbient() ), i );

        math::Vector4f colorIntensity = light->getColor();
        colorIntensity.w              = light->getIntensity();
        lightColorIntensityBinder->write_value(colorIntensity, i);
    }
}

void ForwardRenderer::light_params::setup_point(const scene::Camera& camera,
                                                light_const_iterator firstLight, 
                                                light_const_iterator endLight)
{
    lightCount = std::distance(firstLight, endLight);
    lightCountBinder->write_value(lightCount);
    for (size_t i = 0; firstLight != endLight; ++firstLight, ++i)
    {
        const scene::PointLight* light = static_cast<const scene::PointLight*>(*firstLight);
        lightPositionRadiusBinder->write_value( math::make_vec( math::xyz(light->getPosition()), light->getRadius() ), i );
        lightViewPositionRadiusBinder->write_value( math::make_vec( math::xyz(camera.getViewMatrix() * light->getPosition()), light->getRadius() ), i );

        math::Vector4f colorIntensity = light->getColor();
        colorIntensity.w              = light->getIntensity();
        lightColorIntensityBinder->write_value(colorIntensity, i);
    }
}

    /*
void ForwardRenderer::light_params::setup(int numLights, const scene::SpotLight** lights)
{
    lightCountBinder->write_value(numLights);
    for (int i = 0; i<numLights; ++i)
    {
        lightDirectionAmbientBinder->write_value( math::Vector4f( lights[i]->getDirection(), lights[i]->getAmbient() ), i );
        lightColorIntensityBinder->write_value( math::Vector4f( lights[i]->getColor(), lights[i]->getIntensity() ), i );
    }
}
    */
    
ForwardRenderer::ForwardRenderer(const ForwardRendererDesc& desc_) :
    desc(desc_),
    initialized(false),
    wireframe(false),
    postProcessFormat(sgl::Texture::RGB8)
{
    init();
}

void ForwardRenderer::init()
{
    // create wireframe state
    {
        sgl::RasterizerState::DESC desc;
        desc.cullMode = sgl::RasterizerState::NONE;
        desc.fillMode = sgl::RasterizerState::WIREFRAME;
        wireframeState.reset( currentDevice()->CreateRasterizerState(desc) );
    }

    // add binders
    {
        detail::ParameterTable& parameterTable = detail::currentParameterTable();
        cameraParams.reset( new camera_params(parameterTable) );
        lightParams.reset( new light_params(parameterTable, 3) );


        // create rest parameters
        inputMapBinder = parameterTable.addParameterBinding<sgl::Texture2D>( hash_string("inputMap"), 0, 1, false);
        if (desc.makeDepthMap) {
            depthMapBinder = parameterTable.addParameterBinding<sgl::Texture2D>( hash_string("depthMap"), 0, 1, false);
        }
    }

    initialized = true;
}

void ForwardRenderer::render(realm::World& world, const scene::Camera& camera) const
{
    using namespace scene;

    if (!initialized) {
        const_cast<ForwardRenderer*>(this)->init();
    }

    // render for each camera
    preRenderSignal(camera);

    // get reflect pass resources
    render_group_handle renderGroup = RG_MAIN;
    sgl::Texture2D* reflectRT = 0;
    if ( const scene::ReflectCamera* reflectCamera = dynamic_cast<const scene::ReflectCamera*>(&camera) )
    {
        renderGroup = RG_REFLECT;
        reflectRT   = static_cast<sgl::Texture2D*>( reflectCamera->getRenderTarget()->ColorAttachment(0) );
    }

    cameraParams->setup(camera);
    {
        // gather lights && frustum renderables
        gatherer.cv.clear();
        gatherer.cv.setCamera(&camera);
        {
            thread::lock_ptr lock = world.lockForReading();
            world.visitVisible(camera.getFrustum(), gatherer);
        }

        // partition lights by their types
        std::vector<Light::LIGHT_TYPE> lightTypes;
        {
            light_iterator partitionIter = gatherer.cv.beginLight();
            for (size_t i = 0;
                        i != Light::NUM_LIGHT_TYPES && partitionIter != gatherer.cv.endLight();
                        ++i)
            {
                Light::LIGHT_TYPE   lightType = Light::LIGHT_TYPE(i);
                light_iterator      iter = std::partition( partitionIter, gatherer.cv.endLight(), partition_by_type(lightType) );
                if (iter != partitionIter)
                {
                    lightTypes.push_back(lightType);
                    partitionIter = iter;
                }
            }
        }

        // choose render target
        // update post process render target if needed
        sgl::RenderTarget* renderTarget = camera.getRenderTarget();
        if ( !renderTarget && !camera.getPostEffectChain().empty() )
        {
            sgl::rectangle viewport = camera.getViewport();
            postProcessRenderTarget.reset( updatePPURenderTarget( postProcessRenderTarget.get(),
                                                                  postProcessFormat,
                                                                  viewport.width,
                                                                  viewport.height,
                                                                  true ) );
            renderTarget = postProcessRenderTarget;
        }

        if (renderTarget && sgl::SGL_OK != renderTarget->Bind())
        {
            AUTO_LOGGER_MESSAGE(log::S_ERROR, "Can't bind cameras render target\n");
            return;
        }

        // perform forward rendering
        sgl::Device* device = currentDevice();
        {
            device->SetViewport( camera.getViewport() );

            if (wireframe)
            {
                device->PushState(sgl::State::RASTERIZER_STATE);
                wireframeState->Bind();
            }

            // set params
            inputMapBinder->switch_values(0, 1, false);
            if (depthMapBinder) {
                depthMapBinder->switch_values(0, 1, false);
            }

            // render early-z pass
            if (desc.useDepthPass)
            {
                if (renderTarget) 
                {
                    renderTarget->SetDrawBuffers(0, 0);
                    device->Clear(false, true, false);
                }

                render_pass( renderGroup, RP_DEPTH, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );

                // setup input textures
                if (desc.makeDepthMap && depthMapBinder && renderTarget) {
                    depthMapBinder->switch_values( static_cast<sgl::Texture2D*>( renderTarget->DepthStencilAttachment() ), 1, false );
                }
            }
            else if (desc.makeDepthMap) {
                // Do copy
            }

            // render lightened objects
            {
                if (renderTarget) 
                {
                    renderTarget->SetDrawBuffer(0);
                    bool clearDepth = !desc.useDepthPass;
                    device->Clear(true, clearDepth, false);
                }

                // render scene for each light source
                light_const_iterator lightIter    = gatherer.cv.beginLight();
                light_const_iterator lightIterEnd = gatherer.cv.beginLight();
                size_t i = 0;
                while ( lightIter != gatherer.cv.endLight() ) 
                {
                    if ( lightIterEnd == gatherer.cv.endLight()
                         || std::distance(lightIter, lightIterEnd) == lightParams->max_light_count() 
                         || (*lightIterEnd)->getLightType() != lightTypes[i] ) 
                    {
                        // setup params
                        switch (lightTypes[i])
                        {
                        case scene::Light::DIRECTIONAL:
                            lightParams->setup_directional(camera, lightIter, lightIterEnd);
                            render_pass( renderGroup, RP_DIRECTIONAL_LIGHTING, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );
                            break;
                                                
                        case scene::Light::POINT:
                            lightParams->setup_point(camera, lightIter, lightIterEnd);
                            render_pass( renderGroup, RP_POINT_LIGHTING, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );
                            break;

                        case scene::Light::SPOT:
                            //lightParams->setup(lightIter, lightIterEnd);
                            render_pass( renderGroup, RP_SPOT_LIGHTING, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );
                            break;

                        default:
                            assert(!"Unsupported light type");
                        }

                        lightIter = lightIterEnd;
                        ++i;
                    }
                    else {
                        ++lightIterEnd;
                    }
                }
            }

            // render non lightened objects
            render_pass( renderGroup, RP_OPAQUE, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );

            if (desc.useDebugRender) {
                render_pass( renderGroup, RP_DEBUG, gatherer.cv.beginRenderable(), gatherer.cv.endRenderable() );
            }
        }

        if (wireframe) {
            device->PopState(sgl::State::RASTERIZER_STATE);
        }

        // perform post effect chain
        const filter_chain& postEffects = camera.getPostEffectChain();
        if ( !postEffects.empty() )
        {
            sgl::Texture2D* attachments[] =
            {
                static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(0) ),
                static_cast<sgl::Texture2D*>( renderTarget->ColorAttachment(1) )
            };

            unsigned pingPongSource = 0;
            for (filter_chain::const_iterator iter  = postEffects.begin();
                                              iter != postEffects.end();
                                              ++iter)
            {
                inputMapBinder->switch_values(attachments[pingPongSource], 1, false);
                if (*iter == postEffects.back() && renderTarget == postProcessRenderTarget)
                {
                    renderTarget->Unbind();
                    (*iter)->perform(0, pingPongSource);
                }
                else
                {
                    unsigned numSwitches = (*iter)->perform(camera.getRenderTarget(), pingPongSource);
                    pingPongSource       = (pingPongSource + numSwitches) % 2;
                }
            }
        }

        postRenderSignal(camera);

        if (device->CurrentRenderTarget() != 0) {
            renderTarget->Unbind();
        }

        if (reflectRT) {
            reflectRT->GenerateMipmap();
        }
    }
}

void ForwardRenderer::render_pass(render_group_handle        renderGroup,
                                  render_pass_handle         renderPass,
                                  renderable_const_iterator  firstRenderable, 
                                  renderable_const_iterator  endRenderable) const
{
    // gather packets
    renderPackets.clear();
    graphics::Pass* passes[Effect::MAX_NUM_PASSES];
    for (renderable_const_iterator renderable  = firstRenderable; 
                                   renderable != endRenderable; 
                                   ++renderable)
    {
        int numPasses = (*renderable)->getEffect()->present(renderGroup, renderPass, passes) ;
        for (int i = 0; i<numPasses; ++i) {
            renderPackets.push_back( render_packet(passes[i], *renderable) );
        }
    }

    // sort packets
    std::sort( renderPackets.begin(), renderPackets.end(), sort_by_priority() );

    // render packets
    sgl::Device* device = currentDevice();
    device->PushState(sgl::State::BLEND_STATE);
    device->PushState(sgl::State::DEPTH_STENCIL_STATE);
    device->PushState(sgl::State::RASTERIZER_STATE);
    for (render_packet_iterator iter  = renderPackets.begin();
                                iter != renderPackets.end();
                                ++iter)
    {
        iter->pass->begin();
        iter->renderable->render();
        iter->pass->end();
    }
    device->PopState(sgl::State::BLEND_STATE);
    device->PopState(sgl::State::DEPTH_STENCIL_STATE);
    device->PopState(sgl::State::RASTERIZER_STATE);
}

long long ForwardRenderer::makePriority(RENDER_BIN bin, const void* programPtr)
{
    long long priority = (long)bin << (sizeof(long) / 8)
                       | (long)programPtr;
    return priority;
}

} // namespace detail
} // namespace graphics
} // namespace slon
