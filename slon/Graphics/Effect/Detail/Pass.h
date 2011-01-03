#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_PASS_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_PASS_H__

#include "../Pass.h"
#include "EffectShaderProgram.h"
#include "UniformBinder.h"
#include <sgl/BlendState.h>
#include <sgl/DepthStencilState.h>
#include <sgl/RasterizerState.h>

namespace slon {
namespace graphics {
namespace detail {

/* Technique performing object lighting using shaders */
class Pass :
    public graphics::Pass
{
public:
    typedef std::vector<uniform_binder_ptr>         uniform_binder_vector;
    typedef std::vector<sampler_binder_ptr>         sampler_binder_vector;
    typedef uniform_binder_vector::iterator         uniform_binder_iterator;
    typedef sampler_binder_vector::iterator         sampler_binder_iterator;
    typedef uniform_binder_vector::const_iterator   uniform_binder_const_iterator;
    typedef sampler_binder_vector::const_iterator   sampler_binder_const_iterator;

public:
    struct UNIFORM_DESC
    {
        const char*                         uniformName;
        const char*                         parameterName;
        const abstract_parameter_binding*   parameter;

        UNIFORM_DESC() :
            uniformName(0),
            parameterName(0),
            parameter(0)
        {}
    };

    struct DESC
    {
        const sgl::Program*             program;
        const sgl::BlendState*          blendState;
        const sgl::DepthStencilState*   depthStencilState;
        const sgl::RasterizerState*     rasterizerState;
        std::vector<UNIFORM_DESC>       uniforms;
        long long                       priority;

        DESC() :
            program(0),
            blendState(0),
            depthStencilState(0),
            rasterizerState(0),
            priority(-1)
        {}
    };

public:
    Pass();
    Pass(const DESC& desc);

    /** Setup program to the pass. */
    void setProgram(const sgl::Program* program);

    /** Setup blend state to the pass. */
    void setBlendState(const sgl::BlendState* blendState);

    /** Setup depth stencil state to the pass. */
    void setDepthStencilState(const sgl::DepthStencilState* depthStencilState);

    /** Setup rasterizer state to the pass. */
    void setRasterizerState(const sgl::RasterizerState* rasterizerState);

    /** Get pass shader program. */
    const sgl::Program* getProgram() const { return program.get(); }

    /** Get pass blend state. */
    const sgl::BlendState* getBlendState() const { return blendState.get(); }

    /** Get pass blend state. */
    const sgl::DepthStencilState* getDepthStencilState() const { return depthStencilState.get(); }

    /** Get pass blend state. */
    const sgl::RasterizerState* getRasterizerState() const { return rasterizerState.get(); }

    /** Setup uniform with its binder to the pass.
     * @param uniform - uniform for binding.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkUniform(sgl::Uniform<T>*               uniform,
                     const parameter_binding<T>*    parameter);

    /** Setup uniform with its binder to the pass.
     * @param uniform - uniform for binding.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkUniform(sgl::SamplerUniform<T>*        uniform,
                     const parameter_binding<T>*    parameter);

    // Override Pass
    long long   getPriority() const { return priority; }
    void        begin() const;
    void        end() const;

private:
    uniform_binder_vector                       uniformBinders;
    sampler_binder_vector                       samplerBinders;
    long long                                   priority;
    sgl::ref_ptr<const sgl::Program>            program;
    sgl::ref_ptr<const sgl::BlendState>         blendState;
    sgl::ref_ptr<const sgl::DepthStencilState>  depthStencilState;
    sgl::ref_ptr<const sgl::RasterizerState>    rasterizerState;
};

typedef boost::intrusive_ptr<Pass>          pass_ptr;
typedef boost::intrusive_ptr<const Pass>    const_pass_ptr;

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_PASS_H__
