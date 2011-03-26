#ifndef __SLON_ENGINE_LOG_FORMATTERS_H__
#define __SLON_ENGINE_LOG_FORMATTERS_H__

#include "../Config.h"
#include "Stream.h"
#include <iomanip>
#include <sgl/Math/Matrix.hpp>
#include <sgl/Math/Vector.hpp>

namespace slon {
namespace log {

template<typename T>
struct detailed_fmt;

template<typename T>
struct brief_fmt;

template<typename T, int n, int m>
struct detailed_fmt< math::Matrix<T, n, m> >
{
    typedef math::Matrix<T, n, m> matrix;

    detailed_fmt(const matrix& mat_,
                 bool          decorated_ = false,
                 bool          fixed_ = true,
                 unsigned      width_ = 6)
    :   mat(mat_)
    ,   decorated(decorated_)
    ,   fixed(fixed_)
    ,   width(width_)
    {}

    friend std::ostream& operator << (std::ostream &os, const detailed_fmt& d)
    {
        if (d.decorated) {
            os << "{\n" << indent();
        }

        for (int i = 0; i<n; ++i)
        {
            for (int j = 0; j<m; ++j)
            {
                if (d.fixed) {
                    os << std::fixed << std::setw(d.width) << d.mat[i][j] << " ";
                }
                else {
                    os << std::scientific << d.mat[i][j] << " ";
                }
            }

            if (i != n - 1) {
                os << std::endl;
            }
        }

        if (d.decorated) {
            os << unindent() << "\n}\n";
        }

        return os;
    }

    const matrix& mat;
    bool          decorated;
    bool          fixed;
    unsigned      width;
};

template<typename T, int m>
struct detailed_fmt< math::Matrix<T, 1, m> >
{
    typedef math::Matrix<T, 1, m> matrix;

    detailed_fmt(const matrix& mat_,
                 bool          decorated_ = false,
                 bool          fixed_ = true,
                 unsigned      width_ = 6)
    :   mat(mat_)
    ,   decorated(decorated_)
    ,   fixed(fixed_)
    ,   width(width_)
    {}

    friend std::ostream& operator << (std::ostream &os, const detailed_fmt& d)
    {
        if (d.decorated) {
            os << "{ ";
        }

        for (int j = 0; j<m; ++j)
        {
            if (d.fixed) {
                os << std::fixed << std::setw(d.width) << d.mat[j] << " ";
            }
            else {
                os << std::scientific << d.mat[j] << " ";
            }
        }

        if (d.decorated) {
            os << "}";
        }

        return os;
    }

    const matrix& mat;
    bool          decorated;
    bool          fixed;
    unsigned      width;
};

template<typename T, int n>
struct detailed_fmt< math::Matrix<T, n, 1> >
{
    typedef math::Matrix<T, n, 1> matrix;

    detailed_fmt(const matrix& mat_,
                 bool          decorated_ = false,
                 bool          fixed_ = true,
                 unsigned      width_ = 6)
    :   mat(mat_)
    ,   decorated(decorated_)
    ,   fixed(fixed_)
    ,   width(width_)
    {}

    friend std::ostream& operator << (std::ostream &os, const detailed_fmt& d)
    {
        if (d.decorated) {
            os << "transpose { ";
        }

        for (int j = 0; j<n; ++j)
        {
            if (d.fixed) {
                os << std::fixed << std::setw(d.width) << d.mat[j] << " ";
            }
            else {
                os << std::scientific << d.mat[j] << " ";
            }
        }

        if (d.decorated) {
            os << "}";
        }

        return os;
    }

    const matrix& mat;
    bool          decorated;
    bool          fixed;
    unsigned      width;
};

template<typename T, int n, int m>
detailed_fmt< math::Matrix<T, n, m> > detailed(const math::Matrix<T, n, m>& mat,
                                               bool                         decorated = false,
                                               bool                         fixed = true,
                                               unsigned                     width = 6)
{
    return detailed_fmt< math::Matrix<T, n, m> >(mat, decorated, fixed, width);
}

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_FORMATTERS_H__
