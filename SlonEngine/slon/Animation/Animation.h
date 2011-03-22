#ifndef __SLON_ENGINE_ANIMATION_ANIMATION_H__
#define __SLON_ENGINE_ANIMATION_ANIMATION_H__

#include "../Thread/Timer.h"
#include "AnimationController.h"

namespace slon {
namespace animation {

/** Low level animation interface. Every animation have controller, transferring
 * animation matrices to the scene graph transforms, and target transformation node. 
 * Animations can be organized in tree structure, if so, when you play top level animation, 
 * all animation in the tree start playing with same timer. Same when you stop animation.
 */
class Animation :
    public Referenced
{
public:
	/** Get name of the animation */
	virtual std::string getName() const = 0;

    /** Get number of child animations. */
    virtual size_t getNumAnimations() const = 0;

    /** Get child animation by index. */
    virtual const Animation* getAnimation(size_t index) const = 0;

	/** Get animation controller */
	virtual const AnimationController* getController() const = 0;
	
	/** Get animation target transform */
    virtual const scene::MatrixTransform* getTarget() const = 0;

	/** Play animation with custom timer controlling animation playback. */
	virtual void play(const Timer* timer) = 0;

	/** Stop animation playback, remove timer. */
	virtual void stop() = 0;

	virtual ~Animation() {}
};

} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_ANIMATION_H__
