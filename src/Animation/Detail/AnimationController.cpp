#include "stdafx.h"
#include "Animation/Detail/AnimationController.h"
#include <sgl/Math/MatrixFunctions.hpp>

namespace {

	static math::Matrix4f identity_matrix = math::make_identity<float, 4>();

    template<typename T>
    inline T clamp(T x, T a, T b)
    {
        return x < a ? a : (x > b ? b : x);
    }

} // anonymous namespace

namespace slon {
namespace animation {
namespace detail {

AnimationController::AnimationController()
:   threshold(1E-3f)
{
    transform    = math::make_identity<float, 4>();
    invTransform = math::make_identity<float, 4>();
}

void AnimationController::setTrack(const AnimationTrack* track_)
{
    track.reset(track_);
    if (timer && track) 
    {
        lastTime = timer->getTime();
        frame    = 0;
        advance( (float)lastTime );
    }
}

void AnimationController::setTimer(const Timer* timer_)
{
    timer.reset(timer_);
    if (timer && track) 
    {
        lastTime = timer->getTime();
        frame    = 0;
        advance( (float)lastTime );
    }
}

void AnimationController::advance(float time) const
{
    assert(track && track->getNumFrames() > frame);
    
    // rewind backward
    if ( track->getTime(frame) > time && frame > 0 )
    {
        --frame;
        while ( track->getTime(frame) > time && frame > 0 ) {
            --frame;
        }
    }

    // rewind forward
	while ( track->getTime(frame) < time && frame < (track->getNumFrames() - 1) ) {
        ++frame;
    }
}
	
// Override TransformController
bool AnimationController::dirty() const
{
	return timer && track && abs(timer->getTime() - lastTime) > threshold;
}

void AnimationController::operator () (scene::Node& node, scene::TraverseVisitor& /*tv*/)
{
	if (timer && track) 
	{
		double time  = timer->getTime();
        float  ftime = (float)time;
        advance(ftime);
        lastTime = time;

        float fstart = track->getTime(frame - 1);
        float fend   = track->getTime(frame);
        float value  = clamp( (ftime - fstart) / (fend - fstart), 0.0f, 1.0f );

        scene::MatrixTransform& mt = static_cast<scene::MatrixTransform&>(node);
        mt.setTransformAndInverse( track->getTransform(frame - 1, value), 
                                   track->getInverseTransform(frame - 1, value) );
	}
}
	
} // namespace detail
} // namespace animation
} // namespace slon
