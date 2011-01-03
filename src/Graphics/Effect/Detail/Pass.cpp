#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Effect/Detail/Pass.h"
#include "Graphics/ParameterBinding.h"
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/bind.hpp>

__DEFINE_LOGGER__("graphics.detail.Pass")

namespace {

    using namespace slon;

    sgl::ref_ptr<sgl::BlendState>           defaultBlendState;
    sgl::ref_ptr<sgl::DepthStencilState>    defaultDepthStencilState;
    sgl::ref_ptr<sgl::RasterizerState>      defaultRasterizerState;

    sgl::BlendState* createDefaultBlendState()
    {
        if (!defaultBlendState)
        {
            sgl::BlendState::DESC desc;
            desc.blendEnable = false;
            defaultBlendState.reset( graphics::currentDevice()->CreateBlendState(desc) );
        }

        return defaultBlendState.get();
    }

    sgl::DepthStencilState* createDefaultDepthStencilState()
    {
        if (!defaultDepthStencilState)
        {
            sgl::DepthStencilState::DESC desc;
            desc.depthEnable    = true;
            desc.depthFunc      = sgl::DepthStencilState::LEQUAL;
            desc.depthWriteMask = true;
            desc.stencilEnable  = false;
            defaultDepthStencilState.reset( graphics::currentDevice()->CreateDepthStencilState(desc) );
        }

        return defaultDepthStencilState.get();
    }

    sgl::RasterizerState* createDefaultRasterizerState()
    {
        if (!defaultRasterizerState)
        {
            sgl::RasterizerState::DESC desc;
            desc.cullMode  = sgl::RasterizerState::BACK;
            desc.fillMode  = sgl::RasterizerState::SOLID;
            desc.colorMask = sgl::RasterizerState::RGBA;
            defaultRasterizerState.reset( graphics::currentDevice()->CreateRasterizerState(desc) );
        }

        return defaultRasterizerState.get();
    }
}

namespace slon {
namespace graphics {
namespace detail {

Pass::Pass()
{
}

Pass::Pass(const DESC& desc)
{
    program.reset(desc.program);
    blendState.reset(desc.blendState ? desc.blendState : createDefaultBlendState());
    depthStencilState.reset(desc.depthStencilState ? desc.depthStencilState : createDefaultDepthStencilState());
    rasterizerState.reset(desc.rasterizerState ? desc.rasterizerState : createDefaultRasterizerState());

    for (size_t i = 0; i<desc.uniforms.size(); ++i)
    {
        sgl::AbstractUniform*               uniform    = program->GetUniform(desc.uniforms[i].uniformName);
        const abstract_parameter_binding*   parameter  = desc.uniforms[i].parameter ?
                                                         desc.uniforms[i].parameter :
                                                         currentParameterTable().getParameterBinding( unique_string(desc.uniforms[i].parameterName) ).get();
        bool                                compatible = false;
        if (uniform && parameter)
        {
            switch ( uniform->Type() )
            {
                case sgl::AbstractUniform::INT:
                {
                    const binding_int* typedParameter = dynamic_cast<const binding_int*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::UniformI*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC2I:
                {
                    const binding_vec2i* typedParameter = dynamic_cast<const binding_vec2i*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform2I*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC3I:
                {
                    const binding_vec3i* typedParameter = dynamic_cast<const binding_vec3i*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform3I*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC4I:
                {
                    const binding_vec4i* typedParameter = dynamic_cast<const binding_vec4i*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform4I*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::FLOAT:
                {
                    const binding_float* typedParameter = dynamic_cast<const binding_float*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::UniformF*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC2F:
                {
                    const binding_vec2f* typedParameter = dynamic_cast<const binding_vec2f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform2F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC3F:
                {
                    const binding_vec3f* typedParameter = dynamic_cast<const binding_vec3f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform3F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::VEC4F:
                {
                    const binding_vec4f* typedParameter = dynamic_cast<const binding_vec4f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform4F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::MAT2x2F:
                {
                    const binding_mat2x2f* typedParameter = dynamic_cast<const binding_mat2x2f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform2x2F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::MAT3x3F:
                {
                    const binding_mat3x3f* typedParameter = dynamic_cast<const binding_mat3x3f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform3x3F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::MAT4x4F:
                {
                    const binding_mat4x4f* typedParameter = dynamic_cast<const binding_mat4x4f*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::Uniform4x4F*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::SAMPLER_2D:
                {
                    const binding_tex_2d* typedParameter = dynamic_cast<const binding_tex_2d*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::SamplerUniform2D*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::SAMPLER_3D:
                {
                    const binding_tex_3d* typedParameter = dynamic_cast<const binding_tex_3d*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::SamplerUniform3D*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::SAMPLER_CUBE:
                {
                    const binding_tex_cube* typedParameter = dynamic_cast<const binding_tex_cube*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkUniform( static_cast<sgl::SamplerUniformCube*>(uniform), typedParameter );
                    }
                    break;
                }
                default:
                    logger << log::WL_WARNING << "Uniform '" << desc.uniforms[i].uniformName << "' have unsupported type.\n";
                    break;
            }
        }

        if (!uniform) {
            logger << log::WL_WARNING << "Uniform '" << desc.uniforms[i].uniformName << "' was not loaded.\n";
        }
        else if (!parameter) {
            logger << log::WL_WARNING << "Parameter for uniform '" << desc.uniforms[i].uniformName << "' not specified.\n";
        }
        else if (!compatible) {
            logger << log::WL_WARNING << "Parameter and uniform '" << desc.uniforms[i].uniformName << "' have incompatible types.\n";
        }
    }
}

void Pass::begin() const
{
    sgl::Device* device = currentDevice();

    // bind program
    if (program)
    {
        if ( device->CurrentProgram() != program ) {
            program->Bind();
        }
    }
    else
    {
        if ( device->CurrentProgram() ) {
            device->CurrentProgram()->Unbind();
        }
    }

    // bind uniforms
    std::for_each( boost::make_indirect_iterator( uniformBinders.begin() ),
                   boost::make_indirect_iterator( uniformBinders.end() ),
                   boost::mem_fn(&AbstractUniformBinder::bind) );

    // bind samplers
    unsigned stageOffset = 0;
    for (sampler_binder_const_iterator iter  = samplerBinders.begin();
                                       iter != samplerBinders.end();
                                       ++iter)
    {
        stageOffset += (*iter)->bind(stageOffset);
    }

    // bind states
    if ( device->CurrentBlendState() != blendState ) {
        blendState->Bind();
    }

    if ( device->CurrentDepthStencilState() != depthStencilState ) {
        depthStencilState->Bind();
    }

    if ( device->CurrentRasterizerState() != rasterizerState ) {
        rasterizerState->Bind();
    }
}

void Pass::end() const
{
}

template<typename T>
bool Pass::linkUniform(sgl::Uniform<T>*               uniform,
                       const parameter_binding<T>*    parameter)
{
    typedef boost::intrusive_ptr< uniform_binding<T> > uniform_binding_ptr;

    // find or create uniform binding
    uniform_binding_ptr binding = detail::currentUniformTable().getUniformBinding(uniform);
    if (!binding) {
        binding = detail::currentUniformTable().addUniformBinding(uniform);
    }

    // add uniform binding
    for (uniform_binder_iterator iter  = uniformBinders.begin();
                                 iter != uniformBinders.end();
                                 ++iter)
    {
        if ( UniformBinder<T>* binder = dynamic_cast< UniformBinder<T>* >(iter->get()) )
        {
            binder->addBinding(binding.get(), parameter);
            return true;
        }
    }

    // create binder
    UniformBinder<T>* binder = new UniformBinder<T>();
    binder->addBinding(binding.get(), parameter);
    uniformBinders.push_back(binder);

    return true;
}

template<typename T>
bool Pass::linkUniform(sgl::SamplerUniform<T>*        uniform,
                       const parameter_binding<T>*    parameter)
{
    typedef boost::intrusive_ptr< sampler_uniform_binding<T> > uniform_binding_ptr;

    // find or create uniform binding
    uniform_binding_ptr binding = detail::currentUniformTable().getUniformBinding(uniform);
    if (!binding) {
        binding = detail::currentUniformTable().addUniformBinding(uniform);
    }

    // add uniform binding
    for (sampler_binder_iterator iter  = samplerBinders.begin();
                                 iter != samplerBinders.end();
                                 ++iter)
    {
        if ( SamplerUniformBinder<T>* binder = dynamic_cast< SamplerUniformBinder<T>* >(iter->get()) )
        {
            binder->addBinding(binding.get(), parameter);
            return true;
        }
    }

    // create binder
    SamplerUniformBinder<T>* binder = new SamplerUniformBinder<T>();
    binder->addBinding(binding.get(), parameter);
    samplerBinders.push_back(binder);

    return true;
}

} // namespace detail
} // namespace graphics
} // namespace slon
