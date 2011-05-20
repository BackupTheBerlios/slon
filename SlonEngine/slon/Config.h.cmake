#ifndef __SLON_ENGINE_CONFIG_H__
#define __SLON_ENGINE_CONFIG_H__

#ifdef WIN32
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#endif

#cmakedefine SLON_ENGINE_USE_BULLET
#cmakedefine SLON_ENGINE_USE_SSE2
#cmakedefine SLON_ENGINE_USE_SSE3
#cmakedefine SLON_ENGINE_USE_SSE4
#cmakedefine SLON_ENGINE_USE_GNUPLOT
#cmakedefine SLON_ENGINE_USE_DOUBLE_PRECISION_PHYSICS

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
#	define SLON_ENGINE_FORCE_DEBUG_NEW
#	define SLON_ENGINE_DISABLE_OBJECT_POOL
#endif
#ifdef SLON_ENGINE_FORCE_DEBUG_NEW
#	include "Utility/Memory/debug_new.h"
#endif

#ifdef FINAL_RELEASE
#   define SLON_ENGINE_DISABLE_AUTO_LOGGING
#endif

#endif // __SLON_ENGINE_CONFIG_H__
