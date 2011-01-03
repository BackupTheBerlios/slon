#ifndef __SLON_ENGINE_GRAPHICS_PARAMETER_BINDING_H__
#define __SLON_ENGINE_GRAPHICS_PARAMETER_BINDING_H__

#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <boost/intrusive_ptr.hpp>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace graphics {

class abstract_parameter_binding :
    public referenced
{
protected:
    ~abstract_parameter_binding() {}
};

typedef boost::intrusive_ptr<abstract_parameter_binding>        binding_ptr;
typedef boost::intrusive_ptr<const abstract_parameter_binding>  const_binding_ptr;

template<typename T>
struct parameter_binding :
    public abstract_parameter_binding
{
public:
    parameter_binding(T*        valuesPtr_   = 0,
                      size_t    valuesCount_ = 0,
                      bool      dynamic_     = true) :
        valuesPtr(valuesPtr_),
        valuesCount(valuesCount_),
        useCount(0),
        dynamic(dynamic_)
    {}

    unsigned    count() const               { return valuesCount; }
    unsigned    use_count() const           { return dynamic ? (++useCount) : useCount; }
    const T*    values() const              { return valuesPtr; }
    const T&    value(unsigned i = 0) const { return valuesPtr[i]; }

    void switch_values(T*       valuesPtr_,
                       unsigned valuesCount_,
                       bool     dynamic_)
    {
        valuesPtr   = valuesPtr_;
        valuesCount = valuesCount_;
        dynamic     = dynamic_;
        ++useCount;
    }

    void write_value(const T& value, unsigned offset = 0)
    {
        valuesPtr[offset] = 
            value;
        ++useCount;
    }

    void write_values(const T* values)
    {
        std::copy(values, values + count, valuesPtr);
        ++useCount;
    }

    void write_values(const T*  values,
                      size_t    offset,
                      size_t    writeCount)
    {
        assert(offset + writeCount <= valuesCount);
        std::copy(values + offset, values + offset + writeCount, valuesPtr);
        ++useCount;
    }

    // operators
    const T& operator = (const T& value)
    {
        write_value(value, 0);
        return value;
    }

private:
    // noncopyable
    parameter_binding(const parameter_binding&);
    parameter_binding& operator = (const parameter_binding&);

private:
    T*                  valuesPtr;
    unsigned            valuesCount;
    mutable unsigned    useCount;
    bool                dynamic;
};

#define DEFINE_BINDING(Type, suffix)\
    typedef parameter_binding<Type>                         binding_##suffix;\
    typedef boost::intrusive_ptr<binding_##suffix>          binding_##suffix##_ptr;\
    typedef boost::intrusive_ptr<const binding_##suffix>    const_binding_##suffix##_ptr;

DEFINE_BINDING(math::Matrix4x4f, mat4x4f)
DEFINE_BINDING(math::Matrix4x3f, mat4x3f)
DEFINE_BINDING(math::Matrix4x2f, mat4x2f)
DEFINE_BINDING(math::Matrix3x4f, mat3x4f)
DEFINE_BINDING(math::Matrix3x3f, mat3x3f)
DEFINE_BINDING(math::Matrix3x2f, mat3x2f)
DEFINE_BINDING(math::Matrix2x4f, mat2x4f)
DEFINE_BINDING(math::Matrix2x3f, mat2x3f)
DEFINE_BINDING(math::Matrix2x2f, mat2x2f)

DEFINE_BINDING(math::Vector4f, vec4f)
DEFINE_BINDING(math::Vector3f, vec3f)
DEFINE_BINDING(math::Vector2f, vec2f)
DEFINE_BINDING(float,          float)

DEFINE_BINDING(math::Vector4b, vec4b)
DEFINE_BINDING(math::Vector3b, vec3b)
DEFINE_BINDING(math::Vector2b, vec2b)
DEFINE_BINDING(bool,           bool)

DEFINE_BINDING(math::Vector4i, vec4i)
DEFINE_BINDING(math::Vector3i, vec3i)
DEFINE_BINDING(math::Vector2i, vec2i)
DEFINE_BINDING(int,            int)

DEFINE_BINDING(sgl::Texture,     tex)
DEFINE_BINDING(sgl::Texture2D,   tex_2d)
DEFINE_BINDING(sgl::Texture3D,   tex_3d)
DEFINE_BINDING(sgl::TextureCube, tex_cube)

#undef DEFINE_BINDING

/** Downcast binding*/
template<typename T>
const parameter_binding<T>* cast_binding(const abstract_parameter_binding* binding)
{
    return dynamic_cast<const parameter_binding<T>*>(binding);
}

/** Downcast binding*/
template<typename T>
parameter_binding<T>* cast_binding(abstract_parameter_binding* binding)
{
    return dynamic_cast<parameter_binding<T>*>(binding);
}

/** Downcast binding*/
template<typename T>
boost::intrusive_ptr< const parameter_binding<T> > cast_binding(const boost::intrusive_ptr<const abstract_parameter_binding>& binding)
{
    return boost::intrusive_ptr< const parameter_binding<T> >( dynamic_cast<const parameter_binding<T>*>(binding.get()) );
}

/** Downcast binding*/
template<typename T>
boost::intrusive_ptr< parameter_binding<T> > cast_binding(const boost::intrusive_ptr<abstract_parameter_binding>& binding)
{
    return boost::intrusive_ptr< parameter_binding<T> >( dynamic_cast<parameter_binding<T>*>(binding.get()) );
}

} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_PARAMETER_BINDING_H__
