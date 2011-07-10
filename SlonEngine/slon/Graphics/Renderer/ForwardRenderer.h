#ifndef __SLON_ENGINE_FORWARD_RENDERER_H__
#define __SLON_ENGINE_FORWARD_RENDERER_H__

#include <sgl/Device.h>
#include <vector>
#include "../../Scene/CullVisitor.h"
#include "../Detail/Utility.h"
#include "../Effect/Pass.h"
#include "../Renderer.h"

namespace slon {

// forward decl
namespace scene 
{
    class Light;
    class Camera;
}

namespace graphics {

// forward 
class Renderable;

namespace detail {

// forward
class ParameterTable;

/** ForwardRenderer implements traditional forward rendering shading technique
 */
class ForwardRenderer :
    public Renderer
{
public:
    enum RENDER_BIN
    {
        OPAQUE_BIN,
        BACK_TRANSPARENT_BIN,
        FRONT_TRANSPARENT_BIN
    };

	enum RENDER_GROUP
	{
		RG_MAIN,
		RG_REFLECT
	};

	enum RENDER_PASS
	{
		RP_DEPTH,
		RP_OPAQUE,
		RP_DIRECTIONAL_LIGHTING,
		RP_POINT_LIGHTING,
		RP_SPOT_LIGHTING,
		RP_DEBUG
	};

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

        void setup_directional(const scene::Camera& camera,
                               light_const_iterator firstLight, 
                               light_const_iterator endLight);

        void setup_point(const scene::Camera& camera,
                         light_const_iterator firstLight, 
                         light_const_iterator endLight);

        int max_light_count() const { return maxLightCount; }
    };

    typedef std::auto_ptr<camera_params>                camera_params_ptr;
    typedef std::auto_ptr<light_params>                 light_params_ptr;

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

    // predicate for sorting render packets
    struct sort_by_priority :
        public std::binary_function<const render_packet&, const render_packet&, bool>
    {
        bool operator () (const render_packet& a, const render_packet& b) const 
        {
            return a.pass->getPriority() < b.pass->getPriority();
        }
    };

public:
    ForwardRenderer(const ForwardRendererDesc& desc);

    void toggleWireframe(bool toggle)   { wireframe = toggle; }

    bool isWireframe() const            { return wireframe; }

	// Override Renderer
    void                                handleLight(const scene::Light* light) const;
    RENDER_TECHNIQUE                    getRenderTechnique() const { return FORWARD_RENDERING; }
    void                                render(realm::World& world, const scene::Camera& mainCamera) const;

    /** Make priority for rendering using ForwardRenderer */
    static long long makePriority(RENDER_BIN bin, const void* programPtr);

    ~ForwardRenderer() {}

private:
    void init();

    void render_pass(render_group_handle        renderGroup,
                     render_pass_handle         renderPass,
                     renderable_const_iterator  firstRenderable, 
                     renderable_const_iterator  endRenderable) const;

private:
    // render path setup
    ForwardRendererDesc     desc;
    bool                    initialized;

    // properties
    bool    wireframe;

    // params
    mutable binding_tex_2d_ptr     inputMapBinder;
    mutable binding_tex_2d_ptr     depthMapBinder;

    // sgl
    sgl::Texture::FORMAT                    postProcessFormat;
    sgl::ref_ptr<sgl::RasterizerState>      wireframeState;
    mutable sgl::ref_ptr<sgl::RenderTarget> postProcessRenderTarget;

    // frame
    mutable scene::CullVisitor  cv;
    mutable camera_params_ptr   cameraParams;
    mutable light_params_ptr    lightParams;
    mutable render_packets      renderPackets;
};

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_FORWARD_RENDERER_H__
