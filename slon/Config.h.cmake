#ifndef __SLON_ENGINE_CONFIG_H__
#define __SLON_ENGINE_CONFIG_H__

#cmakedefine SLON_ENGINE_USE_BULLET
#cmakedefine SLON_ENGINE_USE_SSE2
#cmakedefine SLON_ENGINE_USE_SSE3
#cmakedefine SLON_ENGINE_USE_SSE4
#cmakedefine SLON_ENGINE_USE_GNUPLOT

#ifdef SLON_ENGINE_USE_BULLET
#	define SLON_ENGINE_USE_PHYSICS
#endif

#ifdef SLON_ENGINE_USE_GNUPLOT
#   define GNUPLOT_EXECUTABLE "@GNUPLOT_EXECUTABLE@"
#endif

#if defined(SLON_ENGINE_USE_SSE2) || defined(SLON_ENGINE_USE_SSE3) || defined (SLON_ENGINE_USE_SSE4)
#   define SLON_ENGINE_USE_SSE
#endif

#endif // __SLON_ENGINE_CONFIG_H__
