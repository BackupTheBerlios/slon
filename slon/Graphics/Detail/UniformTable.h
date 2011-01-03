#ifndef __SLON_ENGINE_GRAPHICS_DETAIL_UNIFORM_TABLE_H__
#define __SLON_ENGINE_GRAPHICS_DETAIL_UNIFORM_TABLE_H__

#include "ParameterTable.h"
#include <sgl/Uniform.h>

namespace slon {
namespace graphics {
namespace detail {

class abstract_uniform_binding :
    public referenced
{
public:
    virtual ~abstract_uniform_binding() {}
};

typedef boost::intrusive_ptr<abstract_uniform_binding>          uniform_binding_ptr;
typedef boost::intrusive_ptr<const abstract_uniform_binding>    const_uniform_binding_ptr;

template<typename T>
class uniform_binding :
    public abstract_uniform_binding
{
public:
    typedef boost::intrusive_ptr< const parameter_binding<T> >    const_binding_ptr;

public:
    uniform_binding(sgl::Uniform<T>* uniform_ = 0) :
        uniform(uniform_)
    {
        if (uniform) {
            count = uniform->Size();
        }
    }

    void set_uniform(sgl::Uniform<T>* uniform_)
    {
        if (uniform = uniform_) {
            count = uniform->Size();
        }
    }

    sgl::Uniform<T>* get_uniform() 
    { 
        return uniform; 
    }

    void bind(const parameter_binding<T>* valuesBinder_)
    {
        assert( uniform && valuesBinder_ && valuesBinder_->values() );

        if (valuesBinder_ != valuesBinder)
        {
            valuesBinder.reset(valuesBinder_);
            uniform->Set( valuesBinder->values(), valuesBinder->count() > count ? count : valuesBinder->count() );
        }
        else if (valuesBinder->use_count() != valuesUseCount) {
            uniform->Set( valuesBinder->values(), valuesBinder->count() > count ? count : valuesBinder->count() );
        }

        valuesUseCount = valuesBinder->use_count();
    }

    virtual ~uniform_binding() {}

private:
    // noncopyable
    uniform_binding(const uniform_binding&);
    uniform_binding& operator = (const uniform_binding&);

protected:
    sgl::Uniform<T>*    uniform;
    const_binding_ptr   valuesBinder;
    size_t              valuesUseCount;
    size_t              offset;
    size_t              count;
};

template<typename T>
class sampler_uniform_binding :
    public abstract_uniform_binding
{
public:
    typedef boost::intrusive_ptr< const parameter_binding<T> >  const_binding_ptr;

public:
    sampler_uniform_binding(sgl::SamplerUniform<T>* uniform_ = 0) :
        uniform(uniform_)
    {
    }
    
    void set_uniform(sgl::Uniform<T>* uniform_) { uniform = uniform_; }
    sgl::Uniform<T>* get_uniform() { return uniform; }

    void bind(const parameter_binding<T>* valuesBinder_, unsigned stage)
    {
        assert( uniform && valuesBinder_ && valuesBinder_->values() );
        valuesBinder.reset(valuesBinder_);
        uniform->Set( stage, valuesBinder->values() );
    }

    void unbind()
    {
        assert(valuesBinder);
        valuesBinder->values()->Unbind();
    }

    virtual ~sampler_uniform_binding() {}

private:
    // noncopyable
    sampler_uniform_binding(const sampler_uniform_binding&);
    sampler_uniform_binding& operator = (const sampler_uniform_binding&);

protected:
    sgl::SamplerUniform<T>* uniform;
    const_binding_ptr       valuesBinder;
};

class UniformTable :
    public Referenced
{
private:
    template<typename T>
    class uniform_binding :
        public detail::uniform_binding<T>
    {
    public:
        typedef detail::uniform_binding<T> base_type;

    public:
        uniform_binding(UniformTable*       uniformTable_,
                        sgl::Uniform<T>*    uniform) :
            base_type(uniform),
            uniformTable(uniformTable_)
        {
            assert(uniformTable);
        }

        ~uniform_binding()
        {
            assert(uniformTable);
            uniformTable->removeUniformBinding(base_type::uniform);
        }

    private:
        boost::intrusive_ptr<UniformTable> uniformTable;
    };

    template<typename T>
    class sampler_uniform_binding :
        public detail::sampler_uniform_binding<T>
    {
    public:
        typedef detail::sampler_uniform_binding<T> base_type;

    public:
        sampler_uniform_binding(UniformTable*           uniformTable_,
                                sgl::SamplerUniform<T>* uniform) :
            base_type(uniform),
            uniformTable(uniformTable_)
        {
            assert(uniformTable);
        }

        ~sampler_uniform_binding()
        {
            assert(uniformTable);
            uniformTable->removeUniformBinding(base_type::uniform);
        }

    private:
       boost::intrusive_ptr<UniformTable> uniformTable;
    };

    typedef std::map<sgl::AbstractUniform*, abstract_uniform_binding*>  uniform_map;
    typedef uniform_map::iterator                                       uniform_iterator;

public:
    template<typename T>
    boost::intrusive_ptr< uniform_binding<T> > getUniformBinding(sgl::Uniform<T>* uniform)
    {
        uniform_iterator uniformIter = uniforms.find(uniform);
        if ( uniformIter != uniforms.end() ) {
            return boost::intrusive_ptr< uniform_binding<T> >( dynamic_cast<uniform_binding<T>*>(uniformIter->second) );
        }

        return boost::intrusive_ptr< uniform_binding<T> >();
    }

    template<typename T>
    boost::intrusive_ptr< sampler_uniform_binding<T> > getUniformBinding(sgl::SamplerUniform<T>* uniform)
    {
        uniform_iterator uniformIter = uniforms.find(uniform);
        if ( uniformIter != uniforms.end() ) {
            return boost::intrusive_ptr< sampler_uniform_binding<T> >( dynamic_cast<sampler_uniform_binding<T>*>(uniformIter->second) );
        }

        return boost::intrusive_ptr< sampler_uniform_binding<T> >();
    }

    template<typename T>
    boost::intrusive_ptr< uniform_binding<T> > addUniformBinding(sgl::Uniform<T>* uniform)
    {
        boost::intrusive_ptr< uniform_binding<T> > binding( new uniform_binding<T>(this, uniform) );
        if ( uniforms.insert( uniform_map::value_type( uniform, binding.get() ) ).second ) {
            return binding;
        }

        return binding;
    }

    template<typename T>
    boost::intrusive_ptr< sampler_uniform_binding<T> > addUniformBinding(sgl::SamplerUniform<T>* uniform)
    {
        boost::intrusive_ptr< sampler_uniform_binding<T> > binding( new sampler_uniform_binding<T>(this, uniform) );
        if ( uniforms.insert( uniform_map::value_type( uniform, binding.get() ) ).second ) {
            return binding;
        }

        return boost::intrusive_ptr< sampler_uniform_binding<T> >();
    }

private:
    void removeUniformBinding(sgl::AbstractUniform* uniform)
    {
        uniforms.erase(uniform);
    }

private:
    uniform_map uniforms;
};

typedef boost::intrusive_ptr<UniformTable>        uniform_table_ptr;
typedef boost::intrusive_ptr<const UniformTable>  const_uniform_table_ptr;

/** Get UniformTable used by current GraphicsManager. */
UniformTable& currentUniformTable();

} // namespace detail
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_DETAIL_UNIFORM_TABLE_H__
