#include "stdafx.h"
#include "Animation/Detail/Animation.h"

namespace slon {
namespace animation {
namespace detail {

void Animation::play(const Timer* timer)
{
    for (animation_iterator iter  = animations.begin();
                            iter != animations.end();
                            ++iter)
    {
        (*iter)->play(timer);
    }

    if (target && controller) 
    {
        target->setTVCallback( controller.get() );
        controller->setTimer(timer);
    }
}

void Animation::stop()
{
    for (animation_iterator iter  = animations.begin();
                            iter != animations.end();
                            ++iter)
    {
        (*iter)->stop();
    }

    if (target && controller) 
    {
        target->setTVCallback(0);
        controller->setTimer(0);
    }
}

} // namesapce detail
} // namespace animation
} // namespace slon

