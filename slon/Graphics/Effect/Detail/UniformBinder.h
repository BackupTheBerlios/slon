#ifndef __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_BINDER_H__
#define __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_BINDER_H__

#include "../../../Utility/Memory/object_in_pool.hpp"
#include "../../Detail/UniformTable.h"
#include <vector>

namespace slon {
namespace graphics {
namespace detail {

/** Base class for state/uniform/... binders. */
class AbstractUniformBinder :
    public Referenced
{
public:
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
    typedef boost::intrusive_ptr< uniform_binding<T> >                  uniform_binding_ptr;
    typedef boost::intrusive_ptr< const parameter_binding<T> >          parameter_binding_const_ptr;
    typedef std::pair<uniform_binding_ptr, parameter_binding_const_ptr> binding_pair;

    typedef std::vector<binding_pair>               binding_vector;
    typedef typename binding_vector::iterator       binding_iterator;
    typedef typename binding_vector::const_iterator binding_const_iterator;

public:
    /** Add uniform binding if uniform is not yet presented, otherwise replace parameter_binding
     * @param uniformBinding - holder of the uniform for setup.
     * @param parameterBinding - holder of the values for uniform.
     */
    void addBinding(uniform_binding<T>*         uniformBinding,
                    const parameter_binding<T>* parameterBinding);

    /** Remove specified uniform binding. */
    bool removeBinding(uniform_binding<T>* uniformBinding);

    /** Remove all binding pairs. */
    void clear();

    // Override UniformBinder
    void bind() const;

private:
    binding_vector bindings;
};

/** Base class for sampler binders */
class AbstractSamplerBinder :
    public Referenced
{
public:
    /** Bind samplers from stage offset. 
     * @return number of samplers binded.
     */
    virtual unsigned bind(unsigned stageOffset) const = 0;

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
    typedef boost::intrusive_ptr< sampler_uniform_binding<T> >          uniform_binding_ptr;
    typedef boost::intrusive_ptr< const parameter_binding<T> >          parameter_binding_const_ptr;
    typedef std::pair<uniform_binding_ptr, parameter_binding_const_ptr> binding_pair;

    typedef std::vector<binding_pair>               binding_vector;
    typedef typename binding_vector::iterator       binding_iterator;
    typedef typename binding_vector::const_iterator binding_const_iterator;

public:
    /** Add uniform binding if uniform is not yet presented, otherwise replace parameter_binding
     * @param uniformBinding - holder of the uniform for setup.
     * @param parameterBinding - holder of the values for uniform.
     */
    void addBinding(sampler_uniform_binding<T>* uniformBinding,
                    const parameter_binding<T>* parameterBinding);

    /** Remove specified uniform binding. */
    bool removeBinding(sampler_uniform_binding<T>* uniformBinding);

    /** Remove all binding pairs. */
    void clear();

    // Override UniformBinder
    unsigned    bind(unsigned stageOffset) const;
    void        unbind() const;

private:
    binding_vector bindings;
};

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_EFFECT_DETAIL_BINDER_H__
