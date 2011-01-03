#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_FFP_PASS_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_FFP_PASS_H__

#include "../../ParameterBinding.h"
#include "../Pass.h"
#include <sgl/BlendState.h>
#include <sgl/DepthStencilState.h>
#include <sgl/RasterizerState.h>

namespace slon {
namespace graphics {
namespace detail {

/* Technique performing object lighting using shaders */
class FFPPass :
    public graphics::Pass
{
public:
    template<typename T>
    struct PARAMETER_DESC
    {
        const char*                 parameterName;
        const parameter_binding<T>* parameter;

        PARAMETER_DESC() :
            parameterName(0),
            parameter(0)
        {}
    };

    struct DESC
    {
        const sgl::BlendState*          blendState;
        const sgl::DepthStencilState*   depthStencilState;
        const sgl::RasterizerState*     rasterizerState;
        PARAMETER_DESC<math::Matrix4f>  projectionMatrixBinding;
        PARAMETER_DESC<math::Matrix4f>  worldViewMatrixBinding;
        PARAMETER_DESC<math::Vector4f>  diffuseSpecularBinding;
        PARAMETER_DESC<float>           shininessBinding;
        const binding_tex*              textureParameters[8];

        DESC() :
            blendState(0),
            depthStencilState(0),
            rasterizerState(0)
        {
            std::fill(textureParameters, textureParameters + 8, (const binding_tex*)0);
        }
    };

public:
    FFPPass(const DESC& desc);

    /** Setup blend state to the pass. */
    void setBlendState(const sgl::BlendState* blendState);

    /** Setup depth stencil state to the pass. */
    void setDepthStencilState(const sgl::DepthStencilState* depthStencilState);

    /** Setup rasterizer state to the pass. */
    void setRasterizerState(const sgl::RasterizerState* rasterizerState);

    /** Get pass blend state. */
    const sgl::BlendState* getBlendState() const { return blendState.get(); }

    /** Get pass blend state. */
    const sgl::DepthStencilState* getDepthStencilState() const { return depthStencilState.get(); }

    /** Get pass blend state. */
    const sgl::RasterizerState* getRasterizerState() const { return rasterizerState.get(); }

    // Override Pass
    long long   getPriority() const { return 0; }
    void        begin() const;
    void        end() const;

private:
    sgl::ref_ptr<const sgl::BlendState>         blendState;
    sgl::ref_ptr<const sgl::DepthStencilState>  depthStencilState;
    sgl::ref_ptr<const sgl::RasterizerState>    rasterizerState;
    const_binding_mat4x4f_ptr                   projectionMatrixParameter;
    const_binding_mat4x4f_ptr                   worldViewMatrixParameter;
    const_binding_vec4f_ptr                     diffuseSpecularParameter;
    const_binding_float_ptr                     shininessParameter;
    std::vector<const_binding_tex_ptr>          textureParameters;
};

typedef boost::intrusive_ptr<FFPPass>          ffp_pass_ptr;
typedef boost::intrusive_ptr<const FFPPass>    const_ffp_pass_ptr;

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_FFP_PASS_H__
