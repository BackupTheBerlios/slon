#ifndef __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_CONTROLLER_H__
#define __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_CONTROLLER_H__

#include "../../Thread/Timer.h"
#include "../AnimationController.h"
#include "../AnimationTrack.h"

namespace slon {
namespace animation {
namespace detail {

class AnimationController :
	public animation::AnimationController
{
public:
	AnimationController();

    // Override AnimationController
    void setTrack(const AnimationTrack* track);
    void setTimer(const Timer* timer);

    const AnimationTrack* getTrack() const  { return track.get(); }
    const Timer* getTimer() const           { return timer.get(); }

private:
    bool dirty() const;
	void advance(float time) const;

	// Override scene::Node::traverse_visitor_callback
    void operator () (scene::Node& node, scene::TraverseVisitor& tv);

private:
	const_animation_track_ptr	track;
	const_timer_ptr		        timer;

    // playback
	double			        threshold;
	mutable double	        lastTime;
	mutable unsigned	    frame;
    mutable math::Matrix4f  transform;
    mutable math::Matrix4f  invTransform;
};

} // namespace detail
} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_CONTROLLER_H__
