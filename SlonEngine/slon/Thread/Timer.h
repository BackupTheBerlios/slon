#ifndef __SLON_ENGINE_UTILITY_TIMER_H__
#define __SLON_ENGINE_UTILITY_TIMER_H__

#include "../Utility/referenced.hpp"
#include <boost/intrusive_ptr.hpp>

namespace slon {

/** Very simple timer interface */
class Timer :
    public Referenced
{
public:
    /** Get elapsed time in seconds */
    virtual double getTime() const = 0;

    virtual ~Timer() {}
};

typedef boost::intrusive_ptr<Timer>         timer_ptr;
typedef boost::intrusive_ptr<const Timer>   const_timer_ptr;

/** Functor for measuring time intervals */
struct delta_timer
{
    delta_timer() {}
    delta_timer(const Timer* _timer) :
        timer(_timer),
        lastMeasurement( timer->getTime() )
    {}

    /** Get current time - time from last measurement */
    double operator () ()  
    {
        double time     = timer->getTime();
        double delta    = time - lastMeasurement;
        lastMeasurement = time;
        return delta;
    }

    const_timer_ptr timer;
    double          lastMeasurement;
};

} // namespace slon

#endif // __SLON_ENGINE_UTILITY_TIMER_H__
