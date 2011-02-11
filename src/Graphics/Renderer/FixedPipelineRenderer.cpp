#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/Pass.h"
#include "Graphics/Detail/ParameterTable.h"
#include "Graphics/Renderable.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Scene/Visitors/CullVisitor.h"
#include "Scene/Light/DirectionalLight.h"

namespace slon {
namespace graphics {
namespace detail {

FixedPipelineRenderer::camera_params::camera_params(detail::ParameterTable& parameterTable)
{
    normalMatrixBinder          = parameterTable.addParameterBinding( unique_string("normalMatrix"), &normalMatrix, 1, false);
    viewMatrixBinder            = parameterTable.addParameterBinding( unique_string("viewMatrix"), &viewMatrix, 1, false);
    invViewMatrixBinder         = parameterTable.addParameterBinding( unique_string("invViewMatrix"), &invViewMatrix, 1, false);
    projectionMatrixBinder      = parameterTable.addParameterBinding( unique_string("projectionMatrix"), &projectionMatrix, 1, false);
    invProjectionMatrixBinder   = parameterTable.addParameterBinding( unique_string("invProjectionMatrix"), &invProjectionMatrix, 1, false);
    depthParamsBinder           = parameterTable.addParameterBinding( unique_string("depthParams"), &depthParams, 1, false);
    eyePositionBinder           = parameterTable.addParameterBinding( unique_string("eyePosition"), &eyePosition, 1, false);
}

void FixedPipelineRenderer::camera_params::setup(const scene::Camera& camera)
{
    normalMatrixBinder->write_value( camera.getNormalMatrix() );
    viewMatrixBinder->write_value( camera.getViewMatrix() );
    invViewMatrixBinder->write_value( camera.getInverseViewMatrix() );
    projectionMatrixBinder->write_value( camera.getProjectionMatrix() );
    invProjectionMatrixBinder->write_value( math::invert( camera.getProjectionMatrix() ) );
    depthParamsBinder->write_value( math::Vector4f(invProjectionMatrix[2][2], invProjectionMatrix[2][3], invProjectionMatrix[3][2], invProjectionMatrix[3][3]) );
    eyePositionBinder->write_value( math::Vector4f(invViewMatrix[0][3], invViewMatrix[1][3], invViewMatrix[2][3], invViewMatrix[3][3])  );
}

FixedPipelineRenderer::light_params::light_params(detail::ParameterTable& parameterTable, unsigned maxLightCount_) :
    maxLightCount(maxLightCount_),
    lightViewDirectionAmbient(new math::Vector4f[maxLightCount]),
    lightViewPositionRadius(new math::Vector4f[maxLightCount]),
    lightDirectionAmbient(new math::Vector4f[maxLightCount]),
    lightPositionRadius(new math::Vector4f[maxLightCount]),
    lightColorIntensity(new math::Vector4f[maxLightCount])
{
    lightCountBinder                = parameterTable.addParameterBinding( unique_string("lightCount"), &lightCount, 1, false);
    lightViewDirectionAmbientBinder = parameterTable.addParameterBinding( unique_string("lightViewDirectionAmbient"),lightViewDirectionAmbient.get(), maxLightCount, false);
    lightViewPositionRadiusBinder   = parameterTable.addParameterBinding( unique_string("lightViewPositionRadius"), lightViewPositionRadius.get(), maxLightCount, false);
    lightDirectionAmbientBinder     = parameterTable.addParameterBinding( unique_string("lightDirectionAmbient"),lightDirectionAmbient.get(), maxLightCount, false);
    lightPositionRadiusBinder       = parameterTable.addParameterBinding( unique_string("lightPositionRadius"), lightPositionRadius.get(), maxLightCount, false);
    lightColorIntensityBinder       = parameterTable.addParameterBinding( unique_string("lightColorIntensity"), lightColorIntensity.get(), maxLightCount, false);
}

void FixedPipelineRenderer::light_params::setup(int stage, const scene::Light& light)
{
    sgl::Device*     device  = currentDevice();
    sgl::FFPProgram* program = device->FixedPipelineProgram();

    program->GetLightingToggleUniform()->Set(true);
    program->GetLightToggleUniform()->Set(true);
    switch ( light.getLightType() )
    {
        case scene::Light::DIRECTIONAL:
        {
            const scene::DirectionalLight& dLight = static_cast<const scene::DirectionalLight&>(light);
            program->GetLightPositionUniform()->Set( math::make_vec(-dLight.getDirection(), 0.0f) );
            program->GetLightAmbientUniform()->Set( dLight.getColor() * dLight.getAmbient() );
            program->GetLightDiffuseUniform()->Set( dLight.getColor() * dLight.getIntensity() );
            program->GetLightSpecularUniform()->Set( 0.0f * dLight.getColor() * dLight.getIntensity() );
            break;
        }

        default:
            break;
    }
}

FixedPipelineRenderer::FixedPipelineRenderer(const FFPRendererDesc& desc) :
    wireframe(false)
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
    }
}

void FixedPipelineRenderer::render(realm::World& world, const scene::Camera& camera) const
{
    using namespace scene;

    // render for each camera
    preRenderSignal(camera);

    cameraParams->setup(camera);
    {
        // gather lights && frustum renderables
        cullVisitor.clear();
        cullVisitor.setCamera(&camera);
        {
            thread::lock_ptr lock = world.lockForReading();
            world.visit(camera.getFrustum(), cullVisitor);
        }

        // perform forward rendering
        sgl::Device* device = currentDevice();
        device->SetViewport( camera.getViewport() );

        if (wireframe)
        {
            device->PushState(sgl::State::RASTERIZER_STATE);
            wireframeState->Bind();
        }

		const int               maxFFPLights = 8;
        light_const_iterator    lightIter    = cullVisitor.beginLight();
        light_const_iterator    lightIterEnd = cullVisitor.endLight();
		while (lightIter != lightIterEnd)
        {
			// setup lights
            device->FixedPipelineProgram()->GetModelViewMatrixUniform()->Set( camera.getViewMatrix() );
			for (int stage = 0; lightIter != lightIterEnd && stage < maxFFPLights; ++lightIter, ++stage) {
				lightParams->setup(stage, **lightIter);
			}

            // render lightened objects
            render_pass( mainGroupHandle(), lightingPassHandle(), cullVisitor.beginRenderable(), cullVisitor.endRenderable() );
        }

        device->FixedPipelineProgram()->GetLightingToggleUniform()->Set(false);
        render_pass( mainGroupHandle(), opaquePassHandle(), cullVisitor.beginRenderable(), cullVisitor.endRenderable() );
        render_pass( mainGroupHandle(), debugPassHandle(),  cullVisitor.beginRenderable(), cullVisitor.endRenderable() );

        if (wireframe) {
            device->PopState(sgl::State::RASTERIZER_STATE);
        }

        postRenderSignal(camera);
    }
}

void FixedPipelineRenderer::render_pass(render_group_handle        renderGroup,
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

    // render packets
    for (render_packet_iterator iter  = renderPackets.begin();
                                iter != renderPackets.end();
                                ++iter)
    {
        iter->pass->begin();
        iter->renderable->render();
        iter->pass->end();
    }
}

render_group_handle FixedPipelineRenderer::mainGroupHandle()
{
    static render_group_handle handle("Main");
    return handle;
}

render_pass_handle FixedPipelineRenderer::opaquePassHandle()
{
    static render_pass_handle handle("FFPOpaquePass");
    return handle;
}

render_pass_handle FixedPipelineRenderer::lightingPassHandle()
{
    static render_pass_handle handle("FFPLightingPass");
    return handle;
}

render_pass_handle FixedPipelineRenderer::debugPassHandle()
{
    static render_pass_handle handle("DebugPass");
    return handle;
}

} // namespace detail
} // namespace graphics
} // namespace slon