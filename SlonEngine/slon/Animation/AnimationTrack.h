#ifndef __SLON_ENGINE_ANIMATION_ANIMATION_TRACK_H__
#define __SLON_ENGINE_ANIMATION_ANIMATION_TRACK_H__

#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace animation {

class AnimationTrack :
	public Referenced
{
public:
	/** Get interpolated inverse transformation between frames.
     * @param frame - second frame for interpolation.
     * @param value - interpolation value. Must be in [0, 1] segment.
     * @return interpolated transformation between frame and previous frame.
     */
    virtual math::Matrix4f getInverseTransform(unsigned frame, float value) const = 0;

	/** Get inverse transformation of the frame */
    virtual math::Matrix4f getInverseTransform(unsigned frame) const = 0;

	/** Get interpolated transformation between frames.
     * @param frame - second frame for interpolation.
     * @param value - interpolation value. Must be in [0, 1] segment.
     * @return interpolated transformation between frame and previous frame.
     */
    virtual math::Matrix4f getTransform(unsigned frame, float value) const = 0;

	/** Get transformation of the frame */
    virtual math::Matrix4f getTransform(unsigned frame) const = 0;

    /** Get time of the frame. */
    virtual float getTime(unsigned frame) const = 0;

    /** Get number of frames in the animation. */
    virtual unsigned getNumFrames() const = 0;

	virtual ~AnimationTrack() {}
};

} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_ANIMATION_TRACK_H__
