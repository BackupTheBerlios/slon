#ifndef __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_TRACK_H__
#define __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_TRACK_H__

#include "../AnimationTrack.h"
#include <sgl/Math/Quaternion.hpp>
#include <boost/shared_array.hpp>

namespace slon {
namespace animation {
namespace detail {

class AnimationTrack :
    public animation::AnimationTrack
{
public:
    struct frame :
        public sgl::Aligned16
    {
        math::Quaternionf   rotation;
        math::Vector3f      translation;
        float               time;
    };

    struct DESC
    {
        unsigned        numFrames;
        const frame*    frames;
    };

    typedef boost::shared_array<frame> frame_array;

public:
    AnimationTrack(const DESC& desc);

	// Override AnimationTrack
    math::Matrix4f getInverseTransform(unsigned frame, float value) const;
    math::Matrix4f getInverseTransform(unsigned frame) const;

    math::Matrix4f getTransform(unsigned frame, float value) const;
    math::Matrix4f getTransform(unsigned frame) const;

    float getTime(unsigned frame) const;
    unsigned getNumFrames() const;

private:
    unsigned        numFrames;
    frame_array     frames;
};

} // namespace detail
} // namespace animation
} // namespace slon

#endif // __SLON_ENGINE_ANIMATION_DETAIL_ANIMATION_TRACK_H__
