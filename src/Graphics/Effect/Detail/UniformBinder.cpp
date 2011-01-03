#include "stdafx.h"
#include "Graphics/Effect/Detail/UniformBinder.h"

namespace slon {
namespace graphics {
namespace detail {

template<typename T>
void UniformBinder<T>::addBinding(uniform_binding<T>*         uniformBinding,
                                  const parameter_binding<T>* parameterBinding)
{
    assert(uniformBinding);
    assert(parameterBinding);

    // check if not presented
    for (binding_iterator iter  = bindings.begin();
                          iter != bindings.end();
                          ++iter)
    {
        if ( iter->first.get() == uniformBinding ) {
            iter->second.reset(parameterBinding);
        }
    }

    // add binding
    bindings.push_back( binding_pair( uniform_binding_ptr(uniformBinding), 
                                      parameter_binding_const_ptr(parameterBinding) ) );
}

template<typename T>
bool UniformBinder<T>::removeBinding(uniform_binding<T>* uniformBinding)
{
    // remove if presented
    for (binding_iterator iter  = bindings.begin();
                          iter != bindings.end();
                          ++iter)
    {
        if ( iter->first.get() == uniformBinding ) 
        {
            std::swap( *iter, bindings.back() );
            bindings.pop_back();
            return true;
        }
    }

    return false;
}

template<typename T>
void UniformBinder<T>::clear()
{
    bindings.clear();
}

template<typename T>
void UniformBinder<T>::bind() const
{
    for (binding_const_iterator iter  = bindings.begin();
                                iter != bindings.end();
                                ++iter)
    {
        iter->first->bind( iter->second.get() );
    }
}

template<typename T>
void SamplerUniformBinder<T>::addBinding(sampler_uniform_binding<T>* uniformBinding,
                                         const parameter_binding<T>* parameterBinding)
{
    assert(uniformBinding);
    assert(parameterBinding);

    // check if not presented
    for (binding_iterator iter  = bindings.begin();
                          iter != bindings.end();
                          ++iter)
    {
        if ( iter->first.get() == uniformBinding ) {
            iter->second.reset(parameterBinding);
        }
    }

    // add binding
    bindings.push_back( binding_pair( uniform_binding_ptr(uniformBinding), 
                                      parameter_binding_const_ptr(parameterBinding) ) );
}

template<typename T>
bool SamplerUniformBinder<T>::removeBinding(sampler_uniform_binding<T>* uniformBinding)
{
    // remove if presented
    for (binding_iterator iter  = bindings.begin();
                          iter != bindings.end();
                          ++iter)
    {
        if ( iter->first.get() == uniformBinding ) 
        {
            std::swap( *iter, bindings.back() );
            bindings.pop_back();
            return true;
        }
    }

    return false;
}

template<typename T>
void SamplerUniformBinder<T>::clear()
{
    bindings.clear();
}

template<typename T>
unsigned SamplerUniformBinder<T>::bind(unsigned stageOffset) const
{
    unsigned stageOffsetOld = stageOffset;
    for (binding_const_iterator iter  = bindings.begin();
                                iter != bindings.end();
                                ++iter)
    {
        iter->first->bind(iter->second.get(), stageOffset);
        ++stageOffset;
    }

    return stageOffset - stageOffsetOld;
}

template<typename T>
void SamplerUniformBinder<T>::unbind() const
{
    for (binding_const_iterator iter  = bindings.begin();
                                iter != bindings.end();
                                ++iter)
    {
        iter->first->unbind();
    }
}

// explicit template instantiation
template class UniformBinder<float>;
template class UniformBinder<math::Vector2f>;
template class UniformBinder<math::Vector3f>;
template class UniformBinder<math::Vector4f>;

template class UniformBinder<math::Matrix2x2f>;
template class UniformBinder<math::Matrix3x3f>;
template class UniformBinder<math::Matrix4x4f>;

template class UniformBinder<int>;
template class UniformBinder<math::Vector2i>;
template class UniformBinder<math::Vector3i>;
template class UniformBinder<math::Vector4i>;

template class SamplerUniformBinder<sgl::Texture2D>;
template class SamplerUniformBinder<sgl::Texture3D>;
template class SamplerUniformBinder<sgl::TextureCube>;

} // namespace detail
} // namespace graphics
} // namespace slon