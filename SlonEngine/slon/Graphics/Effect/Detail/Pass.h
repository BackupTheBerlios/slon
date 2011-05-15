#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_PASS_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_PASS_H__

#include <sgl/BlendState.h>
#include <sgl/DepthStencilState.h>
#include <sgl/RasterizerState.h>
#include "../../Detail/UniformTable.h"
#include "../Pass.h"
#include "EffectShaderProgram.h"

namespace slon {
namespace graphics {
namespace detail {
	
/** Base class for state/uniform/... binders. */
class AbstractUniformBinder :
    public Referenced
{
public:
	/** Get binding incapsulated by binder. */
	virtual abstract_uniform_binding* getBindingBase() = 0;

	/** Get binding incapsulated by binder. */
	virtual const abstract_uniform_binding* getBindingBase() const = 0;

    /** Bind value to the device. */
    virtual void bind() const = 0;

    virtual ~AbstractUniformBinder() {}
};

typedef boost::intrusive_ptr<AbstractUniformBinder>           uniform_binder_ptr;
typedef boost::intrusive_ptr<const AbstractUniformBinder>     const_uniform_binder_ptr;

/** Binds values to the uniform. */
template<typename T>
class UniformBinder :
    public object_in_pool<UniformBinder<T>, AbstractUniformBinder>
{
private:
    typedef boost::intrusive_ptr< uniform_binding<T> >          uniform_binding_ptr;
    typedef boost::intrusive_ptr< const parameter_binding<T> >	const_parameter_binding_ptr;

public:
	UniformBinder(uniform_binding<T>* binding_ = 0,
                  const parameter_binding<T>* parameter_ = 0)
	:	binding(binding_)
	,	parameter(parameter_)
	{}

	/** Set binding incapsulated by binder. */
	void setBinding(uniform_binding<T>* binding_) { return binding.reset(binding_); }

    /** Get parameter incapsulated by binder. */
    void setParameter(const parameter_binding<T>* parameter_) { return parameter.reset(parameter_); }

	/** Get binding incapsulated by binder. */
	const uniform_binding<T>* getBinding() const { return binding.get(); }

    /** Get parameter incapsulated by binder. */
    const parameter_binding<T>* getParameter() const { return parameter.get(); }

    // Override UniformBinder
	abstract_uniform_binding*       getBindingBase() { return binding.get(); }
	const abstract_uniform_binding* getBindingBase() const { return binding.get(); }
    void                            bind() const { binding->bind( parameter.get() ); }

private:
	uniform_binding_ptr			binding;
    const_parameter_binding_ptr parameter;
};

/** Base class for sampler binders */
class AbstractSamplerBinder :
    public Referenced
{
public:
	/** Get binding incapsulated by binder. */
	virtual abstract_uniform_binding* getBindingBase() = 0;

	/** Get binding incapsulated by binder. */
	virtual const abstract_uniform_binding* getBindingBase() const = 0;

    /** Bind sampler to the specified stage.  */
    virtual void bind(unsigned stage) const = 0;

    /** Unbind samplers. */
    virtual void unbind() const = 0;

    virtual ~AbstractSamplerBinder() {}
};

typedef boost::intrusive_ptr<AbstractSamplerBinder>           sampler_binder_ptr;
typedef boost::intrusive_ptr<const AbstractSamplerBinder>     const_sampler_binder_ptr;

/** Binds values to the uniform. */
template<typename T>
class SamplerUniformBinder :
    public object_in_pool<SamplerUniformBinder<T>, AbstractSamplerBinder>
{
private:
    typedef boost::intrusive_ptr< sampler_uniform_binding<T> > uniform_binding_ptr;
    typedef boost::intrusive_ptr< const parameter_binding<T> > const_parameter_binding_ptr;

public:
	SamplerUniformBinder(sampler_uniform_binding<T>* binding_ = 0,
						 const parameter_binding<T>* parameter_ = 0)
	:	binding(binding_)
	,	parameter(parameter_)
	{}

	/** Set binding incapsulated by binder. */
	void setBinding(sampler_uniform_binding<T>* binding_) { return binding.reset(binding_); }

    /** Get parameter incapsulated by binder. */
    void setParameter(const parameter_binding<T>* parameter_) { return parameter.reset(parameter_); }

	/** Get binding incapsulated by binder. */
	const sampler_uniform_binding<T>* getBinding() const { return binding.get(); }

    /** Get parameter incapsulated by binder. */
    const parameter_binding<T>* getParameter() const { return parameter.get(); }

    // Override UniformBinder
	abstract_uniform_binding*       getBindingBase() { return binding.get(); }
	const abstract_uniform_binding* getBindingBase() const { return binding.get(); }
	void                            bind(unsigned stage) const { binding->bind(parameter.get(), stage); }
    void                            unbind() const { binding->unbind(); }

private:
	uniform_binding_ptr			binding;
    const_parameter_binding_ptr parameter;
};

/* Technique performing object lighting using shaders */
class Pass :
    public object_in_pool<Pass, graphics::Pass>
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
		sgl::AbstractUniform*               uniform;
        const char*                         parameterName;
        const abstract_parameter_binding*   parameter;

        UNIFORM_DESC() :
            uniformName(0),
			uniform(0),
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
        const UNIFORM_DESC*				uniforms;
		size_t							numUniforms;
        long long                       priority;

        DESC() :
            program(0),
            blendState(0),
            depthStencilState(0),
            rasterizerState(0),
			uniforms(0),
			numUniforms(0),
            priority(-1)
        {}
    };

public:
    Pass();
    Pass(const DESC& desc);
    ~Pass();

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

	/** Get number of uniform binders. */
	size_t getNumUniformBinders() const { return uniformBinders.size(); }

	/** Get uniform binder by index */
	AbstractUniformBinder* getUniformBinder(size_t uniform) { return uniformBinders[uniform].get(); }

	/** Get uniform binder by index */
	const AbstractUniformBinder* getUniformBinder(size_t uniform) const { return uniformBinders[uniform].get(); }
	
	/** Get number of sampler uniform binders. */
	size_t getNumSamplerBinders() const { return samplerBinders.size(); }

	/** Get sampler uniform binder by index */
	AbstractSamplerBinder* getSamplerBinder(size_t uniform) { return samplerBinders[uniform].get(); }

	/** Get sampler uniform binder by index */
	const AbstractSamplerBinder* getSamplerBinder(size_t uniform) const { return samplerBinders[uniform].get(); }

    /** Setup uniform with its binder to the pass.
     * @param uniform - index of the uniform.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkUniform(size_t						 uniform,
                     const parameter_binding<T>* parameter);

    /** Setup uniform with its binder to the pass.
     * @param uniform - uniform for binding.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkUniform(sgl::Uniform<T>*               uniform,
                     const parameter_binding<T>*    parameter);
	
    /** Setup uniform with its binder to the pass.
     * @param uniform - index of the uniform.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkSamplerUniform(size_t						uniform,
							const parameter_binding<T>* parameter);

    /** Setup uniform with its binder to the pass.
     * @param uniform - uniform for binding.
     * @param parameter - parameter for binding.
     * @return true if uniform succesfully binded.
     */
    template<typename T>
    bool linkSamplerUniform(sgl::SamplerUniform<T>*     uniform,
							const parameter_binding<T>* parameter);

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
