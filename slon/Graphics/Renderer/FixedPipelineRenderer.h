#ifndef __SLON_ENGINE_GRAPHICS_RENDERER_FIXED_PIPELINE_RENDERER_H__
#define __SLON_ENGINE_GRAPHICS_RENDERER_FIXED_PIPELINE_RENDERER_H__

#include "../../Scene/Visitors/CullVisitor.h"
#include "../Renderer.h"
#include <vector>

namespace slon {

// forward decl
namespace scene 
{
    class Light;
    class Camera;
}

namespace graphics {

// forward 
class Pass;
class Renderable;

namespace detail {

// forward
class ParameterTable;

/** Fixed pipeline is low quality renderer that uses only
 * fixed pipeline and ignores all effects.
 */
class FixedPipelineRenderer :
	public Renderer
{
private:
    typedef scene::CullVisitor::light_iterator              light_iterator;
    typedef scene::CullVisitor::light_const_iterator        light_const_iterator;
    typedef scene::CullVisitor::renderable_iterator         renderable_iterator;
    typedef scene::CullVisitor::renderable_const_iterator   renderable_const_iterator;

    class camera_params :
        public referenced
    {
    private:
        math::Matrix4f  viewMatrix;
        math::Matrix4f  invViewMatrix;
        math::Matrix4f  projectionMatrix;
        math::Matrix4f  invProjectionMatrix;
        math::Matrix3f  normalMatrix;
        math::Vector4f  depthParams;
        math::Vector4f  eyePosition;

        binding_mat3x3f_ptr normalMatrixBinder;
        binding_mat4x4f_ptr viewMatrixBinder;
        binding_mat4x4f_ptr invViewMatrixBinder;
        binding_mat4x4f_ptr projectionMatrixBinder;
        binding_mat4x4f_ptr invProjectionMatrixBinder;
        binding_vec4f_ptr   depthParamsBinder;
        binding_vec4f_ptr   eyePositionBinder;

    public:
        camera_params(detail::ParameterTable& parameterTable);

        void setup(const scene::Camera& camera);
    };

    class light_params :
        public referenced
    {
    private:
        int                                 lightCount;
        int                                 maxLightCount;
        boost::shared_array<math::Vector4f> lightViewDirectionAmbient;
        boost::shared_array<math::Vector4f> lightViewPositionRadius;
        boost::shared_array<math::Vector4f> lightDirectionAmbient;
        boost::shared_array<math::Vector4f> lightPositionRadius;
        boost::shared_array<math::Vector4f> lightColorIntensity;

        binding_int_ptr   lightCountBinder;
        binding_vec4f_ptr lightViewDirectionAmbientBinder;
        binding_vec4f_ptr lightViewPositionRadiusBinder;
        binding_vec4f_ptr lightDirectionAmbientBinder;
        binding_vec4f_ptr lightPositionRadiusBinder;
        binding_vec4f_ptr lightColorIntensityBinder;

    public:
        light_params(detail::ParameterTable& parameterTable, unsigned maxLightCount);

        void setup(int stage, const scene::Light& light);
    };

    typedef std::auto_ptr<camera_params>    camera_params_ptr;
    typedef std::auto_ptr<light_params>     light_params_ptr;

    // render packet
    struct render_packet
    {
        const Pass*         pass;
        const Renderable*   renderable;

        render_packet( const Pass*          pass_,
                       const Renderable*    renderable_ ) :
            pass(pass_),
            renderable(renderable_)
        {}
    };

    typedef std::vector<render_packet>      render_packets;
    typedef render_packets::iterator        render_packet_iterator;

public:
    FixedPipelineRenderer(const FFPRendererDesc& desc);

    void toggleWireframe(bool toggle)   { wireframe = toggle; }

    bool isWireframe() const            { return wireframe; }

	// Override Renderer
    void                handleLight(const scene::Light* light) const;
    RENDER_TECHNIQUE    getRenderTechnique() const { return FIXED_PIPELINE; }
    void                render(realm::World& world, const scene::Camera& mainCamera) const;

    /** Get handle of the opaque pass */
    static render_group_handle mainGroupHandle();

    /** Get handle of the opaque pass */
    static render_pass_handle opaquePassHandle();

    /** Get handle of the lighting pass */
    static render_pass_handle lightingPassHandle();

    /** Get handle of the debug pass */
    static render_pass_handle debugPassHandle();

    virtual ~FixedPipelineRenderer() {}

private:
    void render_pass(render_group_handle        renderGroup,
                     render_pass_handle         renderPass,
                     renderable_const_iterator  firstRenderable, 
                     renderable_const_iterator  endRenderable) const;

private:
    // properties
    bool wireframe;

    // sgl
    sgl::ref_ptr<sgl::RasterizerState> wireframeState;

    // frame
    mutable scene::CullVisitor  cullVisitor;
    mutable camera_params_ptr   cameraParams;
    mutable light_params_ptr    lightParams;
    mutable render_packets      renderPackets;
};

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERER_FIXED_PIPELINE_RENDERER_H__
