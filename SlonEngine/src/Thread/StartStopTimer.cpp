#include "stdafx.h"
#include "Thread/StartStopTimer.h"

namespace {

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MIN
#include <windows.h>

double getTimeInSeconds()
{
	LARGE_INTEGER counter, frequency;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&frequency);
    return static_cast<double>(counter.QuadPart) / frequency.QuadPart;
}
#else
#include <sys/time.h>

double getTimeInSeconds()
{
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec * 1E-6;
}
#endif

} // anonymous namespace

namespace slon {

StartStopTimer::StartStopTimer() :
    ticking(false)
{
}

void StartStopTimer::start()
{
    offset  = getTimeInSeconds();
    ticking = true;
}

// Override timer
double StartStopTimer::getTime() const
{
    return ticking ? getTimeInSeconds() - offset : 0;
}

void StartStopTimer::setTime(double time)
{
    offset = getTimeInSeconds() - time;
}

} // namespace slon