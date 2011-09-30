#ifndef __SLON_ENGINE_MATH_RIGID_TRANSFORM_H__
#define __SLON_ENGINE_MATH_RIGID_TRANSFORM_H__

#include "../Config.h"
#include <sgl/Math/Matrix.hpp>

#ifdef MSVC
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

SGL_BEGIN_MATH_NAMESPACE

/** Represents 4x4 rigid body transformation matrix, i.e. is composition of rotation and translation:
 * | r00 r01 r02 tx |
 * | r10 r11 r12 ty |
 * | r20 r21 r22 tz |
 * | 0   0   0   1  |
 *
 * handles only rotation and translation, but faster to multiply and invert.
 */
template<typename T>
class RigidTransform :
    public Matrix<T, 4, 4>
{
public:
    typedef Matrix<T, 4, 4>                 base_type;
    typedef RigidTransform<T>               this_type;
    typedef typename base_type::row_type    row_type;

public:
    /** Identity by default */
    RigidTransform()
    {
        base_type::rows[0] = row_type(T(1), T(0), T(0), T(0));
        base_type::rows[1] = row_type(T(0), T(1), T(0), T(0));
        base_type::rows[2] = row_type(T(0), T(0), T(1), T(0));
        base_type::rows[3] = row_type(T(0), T(0), T(0), T(1));
    }

	// For internal matrix constructors
    RigidTransform(T _00, T _01, T _02, T _03,
                   T _10, T _11, T _12, T _13,
                   T _20, T _21, T _22, T _23,
                   T _30, T _31, T _32, T _33, T eps = std::numeric_limits<T>::epsilon())
	{
        base_type::rows[0] = row_type(_00, _01, _02, _03);
        base_type::rows[1] = row_type(_10, _11, _12, _13);
        base_type::rows[2] = row_type(_20, _21, _22, _23);
        base_type::rows[3] = row_type(_30, _31, _32, _33);
        if (eps > T(0) && !valid(eps)) {
            throw std::logic_error("matrix is not rigid transform");
        }
	}

    /** Construct transform from matrix, check consistency if needed.
     * @param matrix - matrix to construct from.
     * @param eps - epsilon value, if less or equal than zero then test is not performed. 
     */
    explicit RigidTransform(const base_type& matrix,
                            T                eps = std::numeric_limits<T>::epsilon())
    {
        base_type::rows[0] = matrix.rows[0];
        base_type::rows[1] = matrix.rows[1];
        base_type::rows[2] = matrix.rows[2];
        base_type::rows[3] = matrix.rows[3];
        if (eps > T(0) && !valid(eps)) {
            throw std::logic_error("matrix is not rigid transform");
        }
    }

    /** Construct transform from matrix, check consistency if needed.
     * @param matrix - matrix to construct from.
     * @param eps - epsilon value, if less or equal than zero then test is not performed. 
     */
    template<typename Y>
    explicit RigidTransform(const Matrix<T, 4, 4>& matrix,
                            T                      eps = std::numeric_limits<T>::epsilon())
    {
        base_type::rows[0] = row_type(matrix.rows[0]);
        base_type::rows[1] = row_type(matrix.rows[1]);
        base_type::rows[2] = row_type(matrix.rows[2]);
        base_type::rows[3] = row_type(matrix.rows[3]);
        if (eps > T(0) && !valid(eps)) {
            throw std::logic_error("matrix is not rigid transform");
        }
    }

    /** Check that transform is really rigid body transform, it may became ill due to floating point operations. */
    bool valid(T eps = std::numeric_limits<T>::epsilon()) const
    {
        // check upper left 3x3 orthonormality
        T _00 = base_type::rows[0].x*base_type::rows[0].x + base_type::rows[0].y*base_type::rows[0].y + base_type::rows[0].z*base_type::rows[0].z;
        T _01 = base_type::rows[0].x*base_type::rows[1].x + base_type::rows[0].y*base_type::rows[1].y + base_type::rows[0].z*base_type::rows[1].z;
        T _02 = base_type::rows[0].x*base_type::rows[2].x + base_type::rows[0].y*base_type::rows[2].y + base_type::rows[0].z*base_type::rows[2].z;
        T _11 = base_type::rows[1].x*base_type::rows[1].x + base_type::rows[1].y*base_type::rows[1].y + base_type::rows[1].z*base_type::rows[1].z;
        T _12 = base_type::rows[1].x*base_type::rows[2].x + base_type::rows[1].y*base_type::rows[2].y + base_type::rows[1].z*base_type::rows[2].z;
        T _22 = base_type::rows[2].x*base_type::rows[2].x + base_type::rows[2].y*base_type::rows[2].y + base_type::rows[2].z*base_type::rows[2].z;

        return    fpt_small(_01, eps)
               && fpt_small(_02, eps)
               && fpt_small(_12, eps)
               && fpt_close_abs(_00, T(1), eps)
               && fpt_close_abs(_11, T(1), eps)
               && fpt_close_abs(_22, T(1), eps)
               && fpt_close_abs(base_type::rows[3], row_type(T(0), T(0), T(0), T(1)), eps);
    }

    /** Get translation transformation matrix */
    static this_type translation(T x, T y, T z)
    {
        return this_type( T(1), T(0), T(0), x,
                          T(0), T(1), T(0), y,
                          T(0), T(0), T(1), z,
                          T(0), T(0), T(0), T(1), T(-1) );
    }

    /** Get scaling transformation matrix */
    static this_type scaling(T x, T y, T z)
    {
        return this_type( T(x), T(0), T(0), T(0),
                          T(0), T(y), T(0), T(0),
                          T(0), T(0), T(z), T(0),
                          T(0), T(0), T(0), T(1), T(-1) );
    }

    /** Get rotation around x transformation matrix */
    static this_type rotation_x(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( T(1), T(0), T(0), T(0),
                          T(0), c,    -s,   T(0),
                          T(0), s,    c,    T(0),
                          T(0), T(0), T(0), T(1), T(-1) );
    }

    /** Get rotation around y transformation matrix */
    static this_type rotation_y(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( c,    T(0), s,    T(0),
                          T(0), T(1), T(0), T(0),
                          -s,   T(0), c,    T(0),
                          T(0), T(0), T(0), T(1), T(-1) );
    }

    /** Get rotation around z transformation matrix */
    static this_type rotation_z(T angle)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( c,    -s,   T(0), T(0),
                          s,    c,    T(0), T(0),
                          T(0), T(0), T(1), T(0),
                          T(0), T(0), T(0), T(1), T(-1) );
    }

    /** Get rotation around arbitrary axis transformation matrix */
    static this_type rotation(T angle, const Matrix<T, 3, 1>& v)
    {
        T c = cos(angle);
        T s = sin(angle);
        return this_type( v.x * v.x + (T(1) - v.x * v.x) * c, v.x * v.y * (T(1)- c) - v.z * s,    v.x * v.z * (T(1) - c) + v.y * s,   T(0),
                          v.x * v.y * (T(1) - c) + v.z * s,   v.y * v.y + (T(1) - v.y * v.y) * c, v.y * v.z * (T(1) - c) - v.x * s,   T(0),
                          v.x * v.z * (T(1) - c) - v.y * s,   v.y * v.z * (T(1) - c) + v.x * s,   v.z * v.z + (T(1) - v.z * v.z) * c, T(0),
                          T(0),                               T(0),                               T(0),                               T(1), T(-1) );
    }
};

typedef RigidTransform<float>  RigidTransformf;
typedef RigidTransform<double> RigidTransformd;

/** Invert rigid transform matrix (faster than ordinary matrix invert). */
template<typename T>
inline RigidTransform<T> invert(const RigidTransform<T>& mat)
{
    RigidTransform<T> res;

	res[0][0] =   mat[0][0];
	res[0][1] =   mat[1][0];
	res[0][2] =   mat[2][0];
	res[0][3] = - mat[0][0] * mat[0][3] - mat[1][0] * mat[1][3] - mat[2][0] * mat[2][3];

	res[1][0] =   mat[0][1];
	res[1][1] =   mat[1][1];
	res[1][2] =   mat[2][1];
	res[1][3] = - mat[0][1] * mat[0][3] - mat[1][1] * mat[1][3] - mat[2][1] * mat[2][3];
	
	res[2][0] =   mat[0][2];
	res[2][1] =   mat[1][2];
	res[2][2] =   mat[2][2];
	res[2][3] = - mat[0][2] * mat[0][3] - mat[1][2] * mat[1][3] - mat[2][2] * mat[2][3];
	
	res[3][0] =   mat[3][0];
	res[3][1] =   mat[3][1];
	res[3][2] =   mat[3][2];
	res[3][3] =   mat[3][3];

	return res;
}

// ===================================== SSE ===================================== //
#ifdef SIMPLE_GL_USE_SSE

/** compose homogeneous transforms */
inline RigidTransform<float>& operator *= (RigidTransform<float>& lhs, const RigidTransform<float>& rhs)
{
#ifdef SIMPLE_GL_USE_SSE4
    // load transposed matrix
    __m128 matrix0 = _mm_load_ps(&rhs[0].x);
    __m128 matrix1 = _mm_load_ps(&rhs[1].x);
    __m128 matrix2 = _mm_load_ps(&rhs[2].x);
    __m128 matrix3 = _mm_load_ps(&rhs[3].x);
    _MM_TRANSPOSE4_PS(matrix0, matrix1, matrix2, matrix3);

    __m128 row, dotProd;
    #define __CALC_ROW(i)\
        row         = lhs[i].m128;\
        lhs[i].m128 = _mm_dp_ps(row, matrix0, 0xEE);\
        dotProd     = _mm_dp_ps(row, matrix1, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 1, 1, 1) );\
        dotProd     = _mm_dp_ps(row, matrix2, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 0, 1, 1) );\
        dotProd     = _mm_dp_ps(row, matrix3, 0xEE);\
        lhs[i].m128 = _mm_blend_ps( lhs[i].m128, dotProd, _MM_SHUFFLE(0, 0, 0, 1) );

    // calculate
    __CALC_ROW(0)
    __CALC_ROW(1)
    __CALC_ROW(2)
    // __CALC_ROW(3) - 3rd row should be (0, 0, 0, 1)
    #undef __CALC_ROW

    return lhs;
#else // SSE2
    __m128 row0;
    __m128 row1;
    __m128 row2;
    __m128 row3;
    #define __CALC_ROW(i)\
        row0 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(0, 0, 0, 0) );\
        row1 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(1, 1, 1, 1) );\
        row2 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(2, 2, 2, 2) );\
        row3 = _mm_shuffle_ps( lhs[i].m128, lhs[i].m128, _MM_SHUFFLE(3, 3, 3, 3) );\
        \
        row0 = _mm_mul_ps(row0, rhs[0].m128);\
        row1 = _mm_mul_ps(row1, rhs[1].m128);\
        row2 = _mm_mul_ps(row2, rhs[2].m128);\
        row3 = _mm_mul_ps(row3, rhs[3].m128);\
        \
        lhs[i].m128 = _mm_add_ps(row0, row1);\
        lhs[i].m128 = _mm_add_ps(lhs[i].m128, row2);\
        lhs[i].m128 = _mm_add_ps(lhs[i].m128, row3);

    // calculate
    __CALC_ROW(0)
    __CALC_ROW(1)
    __CALC_ROW(2)
    //__CALC_ROW(3) - 3rd row should be (0, 0, 0, 1)
    #undef __CALC_ROW

    return lhs;
#endif
}

/** transform vector by rigid transform */
inline Matrix<float, 4, 1> operator * (const RigidTransform<float>& mat, const Matrix<float, 4, 1>& vec)
{
#ifdef SIMPLE_GL_USE_SSE4
    __m128 res;
    __m128 dotProd;

    res      = _mm_dp_ps(mat[0].m128, vec.m128, 0xEE);\
    dotProd  = _mm_dp_ps(mat[1].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 1, 1, 1) );\
    dotProd  = _mm_dp_ps(mat[2].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 0, 1, 1) );\
    dotProd  = _mm_dp_ps(mat[3].m128, vec.m128, 0xEE);\
    res      = _mm_blend_ps( res, dotProd, _MM_SHUFFLE(0, 0, 0, 1) );

    return Matrix<float, 4, 1>(res);
#elif defined(SIMPLE_GL_USE_SSE3)
    __m128 res;

    __m128 dotProd0 = _mm_mul_ps(mat[0].m128, vec.m128);
    dotProd0        = _mm_hadd_ps(dotProd0, dotProd0);
    dotProd0        = _mm_hadd_ps(dotProd0, dotProd0);

    __m128 dotProd1 = _mm_mul_ps(mat[1].m128, vec.m128);
    dotProd1        = _mm_hadd_ps(dotProd1, dotProd1);
    dotProd1        = _mm_hadd_ps(dotProd1, dotProd1);

    __m128 dotProd2 = _mm_mul_ps(mat[2].m128, vec.m128);
    dotProd2        = _mm_hadd_ps(dotProd2, dotProd2);
    dotProd2        = _mm_hadd_ps(dotProd2, dotProd2);

    __m128 dotProd3 = _mm_mul_ps(mat[3].m128, vec.m128);
    dotProd3        = _mm_hadd_ps(dotProd3, dotProd3);
    dotProd3        = _mm_hadd_ps(dotProd3, dotProd3);

    __m128 vec01    = _mm_unpacklo_ps(dotProd0, dotProd1);
    __m128 vec23    = _mm_unpackhi_ps(dotProd2, dotProd3);
    res             = _mm_movelh_ps(vec01, vec23);

    return Matrix<float, 4, 1>(res);
#else // SSE2
    // TODO: Think about good sse optimization
    Matrix<float, 4, 1> res;
    res[0] = mat[0][0] * res[0] + mat[0][1] * res[1] + mat[0][2] * res[2] + mat[0][3] * res[3];
    res[1] = mat[1][0] * res[0] + mat[1][1] * res[1] + mat[1][2] * res[2] + mat[1][3] * res[3];
    res[2] = mat[2][0] * res[0] + mat[2][1] * res[1] + mat[2][2] * res[2] + mat[2][3] * res[3];
    res[3] = mat[3][0] * res[0] + mat[3][1] * res[1] + mat[3][2] * res[2] + mat[3][3] * res[3];
    return res;
#endif
}

inline RigidTransform<float>& operator * (const RigidTransform<float>& lhs, const RigidTransform<float>& rhs)
{
    RigidTransform<float> res(lhs);
    return res *= rhs;
}

#endif // SIMPLE_GL_USE_SSE

/** compose rigid transforms */
template<typename T>
RigidTransform<T>& operator *= (RigidTransform<T>& lhs, const RigidTransform<T>& rhs)
{
    // multiply rotations
    T r0 = lhs[0][0]*rhs[0][0] + lhs[0][1]*rhs[1][0] + lhs[0][2]*rhs[2][0];
    T r1 = lhs[0][0]*rhs[0][1] + lhs[0][1]*rhs[1][1] + lhs[0][2]*rhs[2][1];
    T r2 = lhs[0][0]*rhs[0][2] + lhs[0][1]*rhs[1][2] + lhs[0][2]*rhs[2][2];

    lhs[0][0] = r0;
    lhs[0][1] = r1;
    lhs[0][2] = r2;

    r0 = lhs[1][0]*rhs[0][0] + lhs[1][1]*rhs[1][0] + lhs[1][2]*rhs[2][0];
    r1 = lhs[1][0]*rhs[0][1] + lhs[1][1]*rhs[1][1] + lhs[1][2]*rhs[2][1];
    r2 = lhs[1][0]*rhs[0][2] + lhs[1][1]*rhs[1][2] + lhs[1][2]*rhs[2][2];

    lhs[1][0] = r0;
    lhs[1][1] = r1;
    lhs[1][2] = r2;

    r0 = lhs[2][0]*rhs[0][0] + lhs[2][1]*rhs[1][0] + lhs[2][2]*rhs[2][0];
    r1 = lhs[2][0]*rhs[0][1] + lhs[2][1]*rhs[1][1] + lhs[2][2]*rhs[2][1];
    r2 = lhs[2][0]*rhs[0][2] + lhs[2][1]*rhs[1][2] + lhs[2][2]*rhs[2][2];

    lhs[2][0] = r0;
    lhs[2][1] = r1;
    lhs[2][2] = r2;

    // sum translations
    lhs[3][0] += rhs[3][0];
    lhs[3][1] += rhs[3][1];
    lhs[3][2] += rhs[3][2];

    return lhs;
}

template<typename T>
inline RigidTransform<T>& operator * (const RigidTransform<T>& lhs, const RigidTransform<T>& rhs)
{
    RigidTransform<T> res(lhs);
    return res *= rhs;
}

/** transform vector by rigid transform */
template<typename T>
inline Matrix<T, 4, 1> operator * (const RigidTransform<T>& mat, const Matrix<T, 4, 1>& vec)
{
    return Matrix<T, 4, 1>( dot(mat[0], vec), 
                            dot(mat[1], vec),
                            dot(mat[2], vec),
                            vec.w );
}

/** Get rotational term of rigid body transform */
template<typename T>
inline Matrix<T, 3, 3> get_rotation(const RigidTransform<T>& rt)
{
    return Matrix<T, 3, 3>(rt[0][0], rt[0][1], rt[0][2],
                           rt[1][0], rt[1][1], rt[1][2],
                           rt[2][0], rt[2][1], rt[2][2]);
}

/** Get translational term of rigid body transform */
template<typename T>
inline Matrix<T, 3, 1> get_translation(const RigidTransform<T>& rt)
{
    return Matrix<T, 3, 1>(rt[0][3], rt[1][3], rt[2][3]);
}

SGL_END_MATH_NAMESPACE

#ifdef MSVC
#pragma warning( pop )
#endif

#endif // __SLON_ENGINE_MATH_RIGID_TRANSFORM_H__
