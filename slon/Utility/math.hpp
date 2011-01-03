#ifndef __SLON_ENGINE_MATH_H__
#define __SLON_ENGINE_MATH_H__

#include <limits>
#include <sgl/Math/AABB.hpp>

namespace slon {

    template<typename T>
    struct bounds
    {};

    template<typename T>
    struct bounds< math::AABB<T, 3> >
    {
    public:
        static const math::AABB<T, 3>& infinite()        { return infiniteAABB; }
        static const math::AABB<T, 3>& inv_infinite()    { return invInfiniteAABB; }

    private:
        static math::AABB<T, 3> infiniteAABB;
        static math::AABB<T, 3> invInfiniteAABB;
    };

    template<typename T>
    math::AABB<T, 3> bounds< math::AABB<T, 3> >::infiniteAABB = math::AABBf( -std::numeric_limits<T>::max(),
                                                                             -std::numeric_limits<T>::max(),
                                                                             -std::numeric_limits<T>::max(),
                                                                              std::numeric_limits<T>::max(),
                                                                              std::numeric_limits<T>::max(),
                                                                              std::numeric_limits<T>::max() );

    template<typename T>
    math::AABB<T, 3> bounds< math::AABB<T, 3> >::invInfiniteAABB = math::AABBf(  std::numeric_limits<T>::max(),
                                                                                 std::numeric_limits<T>::max(),
                                                                                 std::numeric_limits<T>::max(),
                                                                                -std::numeric_limits<T>::max(),
                                                                                -std::numeric_limits<T>::max(),
                                                                                -std::numeric_limits<T>::max() );

} // namespace slon

#endif // __SLON_ENGINE_MATH_H__
