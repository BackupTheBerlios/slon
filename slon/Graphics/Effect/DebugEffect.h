#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__

#include "TransformEffect.h"

namespace slon {
namespace graphics {

/** Effect for rendering debug information */
class DebugEffect :
    public TransformEffect
{
public:
    DebugEffect();

    /** Setup model matrix for the effect */
    void setModelMatrix(const math::Matrix4f& _modelMatrix);

    /** Setup projection matrix for the effect. */
    void setProjectionMatrix( bool                  useCameraProjection,
                              const math::Matrix4f& projectionMatrix = math::Matrix4f() );

    /** Setup rendering color */
    void setColor(const math::Vector4f& color);

    /** Toggle depth test */
    void toggleDepthTest(bool depthTest);

    /** Toggle wireframe */
    void toggleWireframe(bool wireframe);

    /** Get ambient material color of the effect */
    const math::Matrix4f& getModelMatrix() const { return modelMatrix; }

    /** Get render color */
    const math::Vector4f& getColor() const { return color; }

    /** Get depth test state */
    bool getDepthTestToggle() const { return depthTest; }

    // Override Effectint
    int     present(render_group_handle renderGroup, render_pass_handle renderPass, Pass** passes);
    int     queryAttribute(unique_string /*name*/) { return -1; }

private:
    // properties
    math::Matrix4f      projectionMatrix;
    math::Matrix4f      modelMatrix;
    math::Vector4f      color;
    bool                useCameraProjection;
    bool                depthTest;
    bool                wireframe;

    // binders
    binding_vec4f_ptr   colorBinder;
    binding_mat4x4f_ptr ownProjectionMatrixBinder;

    // technique
    pass_ptr        pass;
};

typedef boost::intrusive_ptr<DebugEffect>       debug_effect_ptr;
typedef boost::intrusive_ptr<const DebugEffect> const_debug_effect_ptr;

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DEBUG_EFFECT_H__
