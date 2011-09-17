#ifndef __SLON_ENGINE_THREAD_START_STOP_TIMER_H__
#define __SLON_ENGINE_THREAD_START_STOP_TIMER_H__

#include "Timer.h"

namespace slon {

/** Timer measuring time from the creation. Uses windows QueryPerformanceCounter,
 * QueryPerformanceFrequency or linux gettimeofday functions.
 */
class SLON_PUBLIC StartStopTimer :
    public Timer
{
public:
    StartStopTimer();

    /** Reset offset to current time and start timer */
    void start();

    /** Stop timer measuring */
    void stop() { ticking = false; }

    /** Check wether timer is ticking */
    bool isTicking() const { return ticking; }

    /** Setup current time. Timer will measure elapsed time after setup. */
    void setTime(double offset);

    // Override timer
    double getTime() const;

private:
    bool    ticking;
    double  offset;
};

typedef boost::intrusive_ptr<StartStopTimer>         start_stop_timer_ptr;
typedef boost::intrusive_ptr<const StartStopTimer>   const_start_stop_timer_ptr;

} // namespace slon

#endif // __SLON_ENGINE_THREAD_START_STOP_TIMER_H__