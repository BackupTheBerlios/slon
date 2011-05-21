#ifndef __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_H__
#define __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_H__

#include <vector>
#include "../Animation.h"

namespace slon {
namespace animation {
namespace detail {

class Animation :
    public animation::Animation
{
public:
    typedef std::vector<animation::animation_ptr> animation_vector;
    typedef animation_vector::iterator            animation_iterator;

public:
    /** Set name of the animation */
    void setName(const std::string& name_) { name = name_; }

    /** Add child animation */
    void addAnimation(animation::Animation* animation) { assert(animation); animations.push_back(animation); }

    /** Setup transformation node for animation */
    void setTarget(scene::MatrixTransform* target_) { target.reset(target_); }

    /** Setup controller transferring matrix to target transform */
    void setController(AnimationController* controller_) { controller.reset(controller_); }

    // Override Animation
	std::string	getName() const			 { return name; }
    size_t		getNumAnimations() const { return animations.size(); }

    const animation::Animation*             getAnimation(size_t index) const { assert(index < animations.size()); return animations[index].get(); }
    const animation::AnimationController*   getController() const { return controller.get(); }
    const scene::MatrixTransform*           getTarget() const { return target.get(); }

	void play(const Timer* timer);
	void stop();

private:
    std::string                     name;
    animation_controller_ptr        controller;
    scene::matrix_transform_ptr     target;
    animation_vector                animations;
};

typedef boost::intrusive_ptr<Animation>			animation_ptr;
typedef boost::intrusive_ptr<const Animation>	const_animation_ptr;

} // namesapce detail
} // namespace animation
} // namespace slon


#endif // __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_H__
