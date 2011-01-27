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

    for (size_t i = 0; i<desc.numUniforms; ++i)
    {
		// Not very good situation, notify may be
		if (!desc.uniforms[i].uniformName && !desc.uniforms[i].uniform) {
			continue;
		}

		// find corresponding uniform <-> parameter pairs, check their types
        sgl::AbstractUniform*             uniform    = desc.uniforms[i].uniform ? 
                                                       desc.uniforms[i].uniform :
                                                       program->GetUniform(desc.uniforms[i].uniformName);
        const abstract_parameter_binding* parameter  = desc.uniforms[i].parameter ?
                                                       desc.uniforms[i].parameter :
                                                       currentParameterTable().getParameterBinding( unique_string(desc.uniforms[i].parameterName) ).get();
        bool                              compatible = false;
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
                        linkSamplerUniform( static_cast<sgl::SamplerUniform2D*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::SAMPLER_3D:
                {
                    const binding_tex_3d* typedParameter = dynamic_cast<const binding_tex_3d*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkSamplerUniform( static_cast<sgl::SamplerUniform3D*>(uniform), typedParameter );
                    }
                    break;
                }
                case sgl::AbstractUniform::SAMPLER_CUBE:
                {
                    const binding_tex_cube* typedParameter = dynamic_cast<const binding_tex_cube*>(parameter);
                    if ( (compatible = (typedParameter != 0)) ) {
                        linkSamplerUniform( static_cast<sgl::SamplerUniformCube*>(uniform), typedParameter );
                    }
                    break;
                }
                default:
                    logger << log::WL_WARNING 
						   << "Uniform '" 
						   << (desc.uniforms[i].uniform ? "..." : desc.uniforms[i].uniformName)
						   << "' have unsupported type.\n";
                    break;
            }
        }

        if (!uniform) 
		{
            logger << log::WL_WARNING 
				   << "Uniform '" 
				   << (desc.uniforms[i].uniform ? "..." : desc.uniforms[i].uniformName) 
				   << "' was not loaded.\n";
        }
        else if (!parameter) 
		{
            logger << log::WL_WARNING 
				   << "Parameter for uniform '" 
				   << (desc.uniforms[i].uniform ? "..." : desc.uniforms[i].uniformName) 
				   << "' not specified.\n";
        }
        else if (!compatible) 
		{
            logger << log::WL_WARNING 
				   << "Parameter and uniform '" 
				   << (desc.uniforms[i].uniform ? "..." : desc.uniforms[i].uniformName)
				   << "' have incompatible types.\n";
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
    unsigned stage = 0;
    for (sampler_binder_const_iterator iter  = samplerBinders.begin();
                                       iter != samplerBinders.end();
                                       ++iter)
    {
        (*iter)->bind(stage++);
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

void Pass::setBlendState(const sgl::BlendState* blendState_)
{
	blendState.reset(blendState_);
}

void Pass::setDepthStencilState(const sgl::DepthStencilState* depthStencilState_)
{
	depthStencilState.reset(depthStencilState_);
}

void Pass::setRasterizerState(const sgl::RasterizerState* rasterizerState_)
{
	rasterizerState.reset(rasterizerState_);
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
		UniformBinder<T>* binder = dynamic_cast< UniformBinder<T>* >(iter->get());
        if ( binder && binder->getBinding() == binding.get() )
        {
			binder->setParameter(parameter);
			return true;
        }
    }

    // create binder
    UniformBinder<T>* binder = new UniformBinder<T>(binding.get(), parameter);
    uniformBinders.push_back(binder);

    return true;
}

template<typename T>
bool Pass::linkUniform(size_t					   uniform,
					   const parameter_binding<T>* parameter)
{
	if ( uniform < uniformBinders.size() ) {
		return false;
	}
	
    if ( UniformBinder<T>* binder = dynamic_cast< UniformBinder<T>* >(uniformBinders[uniform].get()) )
    {
        binder->setParameter(parameter);
        return true;
    }

	return false;
}

template<typename T>
bool Pass::linkSamplerUniform(sgl::SamplerUniform<T>*     uniform,
                              const parameter_binding<T>* parameter)
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
		SamplerUniformBinder<T>* binder = dynamic_cast< SamplerUniformBinder<T>* >(iter->get());
        if ( binder && binder->getBinding() == binding.get() )
        {
			binder->setParameter(parameter);
			return true;
        }
    }

    // create binder
    SamplerUniformBinder<T>* binder = new SamplerUniformBinder<T>(binding.get(), parameter);
    samplerBinders.push_back(binder);

    return true;
}

template<typename T>
bool Pass::linkSamplerUniform(size_t					  uniform,
						      const parameter_binding<T>* parameter)
{
	if ( uniform < samplerBinders.size() ) {
		return false;
	}
	
    if ( SamplerUniformBinder<T>* binder = dynamic_cast< SamplerUniformBinder<T>* >(samplerBinders[uniform].get()) )
    {
        binder->setParameter(parameter);
        return true;
    }

	return false;
}

// explicit template instantiation
template bool Pass::linkUniform<float>(size_t, const parameter_binding<float>*);
template bool Pass::linkUniform<math::Vector2f>(size_t, const parameter_binding<math::Vector2f>*);
template bool Pass::linkUniform<math::Vector3f>(size_t, const parameter_binding<math::Vector3f>*);
template bool Pass::linkUniform<math::Vector4f>(size_t, const parameter_binding<math::Vector4f>*);

template bool Pass::linkUniform<math::Matrix2x2f>(size_t, const parameter_binding<math::Matrix2x2f>*);
template bool Pass::linkUniform<math::Matrix3x3f>(size_t, const parameter_binding<math::Matrix3x3f>*);
template bool Pass::linkUniform<math::Matrix4x4f>(size_t, const parameter_binding<math::Matrix4x4f>*);

template bool Pass::linkUniform<int>(size_t, const parameter_binding<int>*);
template bool Pass::linkUniform<math::Vector2i>(size_t, const parameter_binding<math::Vector2i>*);
template bool Pass::linkUniform<math::Vector3i>(size_t, const parameter_binding<math::Vector3i>*);
template bool Pass::linkUniform<math::Vector4i>(size_t, const parameter_binding<math::Vector4i>*);

template bool Pass::linkSamplerUniform<sgl::Texture2D>(size_t, const parameter_binding<sgl::Texture2D>*);
template bool Pass::linkSamplerUniform<sgl::Texture3D>(size_t, const parameter_binding<sgl::Texture3D>*);
template bool Pass::linkSamplerUniform<sgl::TextureCube>(size_t, const parameter_binding<sgl::TextureCube>*);

template bool Pass::linkUniform<float>(sgl::Uniform<float>*, const parameter_binding<float>*);
template bool Pass::linkUniform<math::Vector2f>(sgl::Uniform<math::Vector2f>*, const parameter_binding<math::Vector2f>*);
template bool Pass::linkUniform<math::Vector3f>(sgl::Uniform<math::Vector3f>*, const parameter_binding<math::Vector3f>*);
template bool Pass::linkUniform<math::Vector4f>(sgl::Uniform<math::Vector4f>*, const parameter_binding<math::Vector4f>*);

template bool Pass::linkUniform<math::Matrix2x2f>(sgl::Uniform<math::Matrix2x2f>*, const parameter_binding<math::Matrix2x2f>*);
template bool Pass::linkUniform<math::Matrix3x3f>(sgl::Uniform<math::Matrix3x3f>*, const parameter_binding<math::Matrix3x3f>*);
template bool Pass::linkUniform<math::Matrix4x4f>(sgl::Uniform<math::Matrix4x4f>*, const parameter_binding<math::Matrix4x4f>*);

template bool Pass::linkUniform<int>(sgl::Uniform<int>*, const parameter_binding<int>*);
template bool Pass::linkUniform<math::Vector2i>(sgl::Uniform<math::Vector2i>*, const parameter_binding<math::Vector2i>*);
template bool Pass::linkUniform<math::Vector3i>(sgl::Uniform<math::Vector3i>*, const parameter_binding<math::Vector3i>*);
template bool Pass::linkUniform<math::Vector4i>(sgl::Uniform<math::Vector4i>*, const parameter_binding<math::Vector4i>*);

template bool Pass::linkSamplerUniform<sgl::Texture2D>(sgl::SamplerUniform<sgl::Texture2D>*, const parameter_binding<sgl::Texture2D>*);
template bool Pass::linkSamplerUniform<sgl::Texture3D>(sgl::SamplerUniform<sgl::Texture3D>*, const parameter_binding<sgl::Texture3D>*);
template bool Pass::linkSamplerUniform<sgl::TextureCube>(sgl::SamplerUniform<sgl::TextureCube>*, const parameter_binding<sgl::TextureCube>*);

} // namespace detail
} // namespace graphics
} // namespace slon
