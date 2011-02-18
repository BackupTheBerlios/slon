#ifndef __SLON_ENGINE_ANIMATION_FORWARD_H__
#define __SLON_ENGINE_ANIMATION_FORWARD_H__

#include "../Config.h"

namespace boost
{
    template<typename T>
    class intrusive_ptr;
}

namespace slon {
namespace animation {

// forward
class Animation;
class AnimationController;
class AnimationTrack;

// ptr typedefs
typedef boost::intrusive_ptr<Animation>					animation_ptr;
typedef boost::intrusive_ptr<const Animation>			const_animation_ptr;
typedef boost::intrusive_ptr<AnimationController>		animation_controller_ptr;
typedef boost::intrusive_ptr<const AnimationController>	const_animation_controller_ptr;
typedef boost::intrusive_ptr<AnimationTrack>			animation_track_ptr;
typedef boost::intrusive_ptr<const AnimationTrack>		const_animation_track_ptr;

} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_FORWARD_H__