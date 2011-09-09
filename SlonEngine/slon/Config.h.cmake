#ifndef __SLON_ENGINE_CONFIG_H__
#define __SLON_ENGINE_CONFIG_H__

#ifdef WIN32
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#endif

#cmakedefine SLON_ENGINE_BUILD_SHARED
#cmakedefine SLON_ENGINE_USE_BULLET
#cmakedefine SLON_ENGINE_USE_PYTHON
#cmakedefine SLON_ENGINE_USE_SSE2
#cmakedefine SLON_ENGINE_USE_SSE3
#cmakedefine SLON_ENGINE_USE_SSE4
#cmakedefine SLON_ENGINE_USE_GNUPLOT
#cmakedefine SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS

#ifdef SLON_ENGINE_BUILD_SHARED
#   ifdef WIN32
#       ifdef SlonEngine_EXPORTS
#           ifdef __GNUC__
#               define SLON_PUBLIC __attribute__ ((dllexport))
#           else
#               define SLON_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#           endif
#       else
#           ifdef __GNUC__
#               define SLON_PUBLIC __attribute__ ((dllimport))
#           else
#               define SLON_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#           endif
#       endif
#       define SLON_LOCAL
#   else
#       if __GNUC__ >= 4
#           define SLON_PUBLIC __attribute__ ((visibility ("default")))
#           define SLON_LOCAL  __attribute__ ((visibility ("hidden")))
#       else
#           define SLON_PUBLIC
#           define SLON_LOCAL
#       endif
#   endif
#else
#   define SLON_PUBLIC
#   define SLON_LOCAL
#endif

#ifdef SLON_ENGINE_USE_BULLET
#	define SLON_ENGINE_USE_PHYSICS
#endif

namespace slon {
namespace physics {
#ifdef SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS
    typedef double real;
#else
    typedef float real;
#endif
} // namespace physics
} // namespace slon

#ifdef SLON_ENGINE_USE_GNUPLOT
#   define GNUPLOT_EXECUTABLE "@GNUPLOT_EXECUTABLE@"
#endif

#if defined(SLON_ENGINE_USE_SSE2) || defined(SLON_ENGINE_USE_SSE3) || defined (SLON_ENGINE_USE_SSE4)
#   define SLON_ENGINE_USE_SSE
#endif

#ifdef _DEBUG
#	define SLON_ENGINE_DISABLE_OBJECT_POOL
#endif

#ifdef FINAL_RELEASE
#   define SLON_ENGINE_DISABLE_AUTO_LOGGING
#endif

#endif // __SLON_ENGINE_CONFIG_H__
