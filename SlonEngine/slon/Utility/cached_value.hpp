#ifndef __SLON_ENGINE_CACHED_VALUE_HPP__
#define __SLON_ENGINE_CACHED_VALUE_HPP__

#include "../Config.h"
#include <sgl/Math/Frustum.hpp>
#ifdef SLON_ENGINE_USE_SSE
#include <sgl/Utility/Aligned.h>
#endif

namespace slon {

template<typename T>
class cached_value
{
public:
    cached_value() :
        dirty(true)
    {
    }

    explicit cached_value(const T& _value) :
        value(_value),
        dirty(false)
    {
    }

    /** Force value recalculation */
    inline void make_dirty() const { dirty = true; } 

    /** Check where value need to be recalculated */
    inline bool is_dirty()   const { return dirty; }

    /** Setup value */
    inline const T& operator = (const T& _value) 
    {
        dirty = false;
        return value = _value; 
    }

    /** Implicit cast to value type */
    inline operator T& () { return value; }

    /** Implicit cast to value type */
    inline operator const T& () const { return value; }

private:
    T               value;
    mutable bool    dirty;
};

typedef cached_value<math::Matrix4f>  CachedMatrix4f;
typedef cached_value<math::Matrix3f>  CachedMatrix3f;
typedef cached_value<math::Frustumf>  CachedFrustumf;

} // namespace slon

#endif // __SLON_ENGINE_CACHED_VALUE_HPP__
