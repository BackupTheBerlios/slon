#ifndef __SLON_ENGINE_ANIMATION_ANIMATION_CONTROLLER_H__
#define __SLON_ENGINE_ANIMATION_ANIMATION_CONTROLLER_H__

#include "../Scene/MatrixTransform.h"
#include "AnimationTrack.h"

namespace slon {
namespace animation {

class AnimationController :
    public Referenced
{
public:
	/** Set animation for playback */
	virtual void setTrack(const AnimationTrack* animation) = 0;

	/** Get current animation */
	virtual const AnimationTrack* getTrack() const = 0;

	/** Set timer controlling animation playback */
	virtual void setTimer(const Timer* timer) = 0;

	/** Get timer controlling animation playback */
	virtual const Timer* getTimer() const = 0;

	virtual ~AnimationController() {}
};

} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_ANIMATION_CONTROLLER_H__
