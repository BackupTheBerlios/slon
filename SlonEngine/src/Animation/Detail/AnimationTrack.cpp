#include "stdafx.h"
#include "Animation/Detail/AnimationTrack.h"
#include <algorithm>
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace animation {
namespace detail {

AnimationTrack::AnimationTrack(const DESC& desc)
{
    assert(desc.numFrames > 0 && desc.frames != 0);
    frames.reset(new frame[desc.numFrames]);
    numFrames = desc.numFrames;
    std::copy( desc.frames, desc.frames + numFrames, frames.get() );
}

// Override AnimationTrack
math::Matrix4f AnimationTrack::getInverseTransform(unsigned frame, float value) const
{
    assert(frame < numFrames - 1 && value >= 0.0f && value <= 1.0f);
 
    math::Quaternionf quat  = math::invert( math::slerp(frames[frame].rotation, frames[frame+1].rotation, value) );
    math::Vector3f    trans = math::lerp(frames[frame].translation, frames[frame+1].translation, value);

    math::Matrix4f inv      = math::to_matrix_4x4(quat);
    math::Vector4f invTrans = inv * math::make_vec(-trans, 0.0f);

    inv[0][3] = invTrans.x;
    inv[1][3] = invTrans.y;
    inv[2][3] = invTrans.z;

    return inv;
}

math::Matrix4f AnimationTrack::getInverseTransform(unsigned frame) const
{
    assert(frame < numFrames);

    const math::Quaternionf& quat  = math::invert(frames[frame].rotation);
    const math::Vector3f&    trans = frames[frame].translation;
    
    math::Matrix4f inv      = math::to_matrix_4x4(quat);
    math::Vector4f invTrans = inv * math::make_vec(-trans, 0.0f);

    inv[0][3] = invTrans.x;
    inv[1][3] = invTrans.y;
    inv[2][3] = invTrans.z;

    return inv;
}

math::Matrix4f AnimationTrack::getTransform(unsigned frame, float value) const
{
    assert(frame < numFrames - 1 && value >= 0.0f && value <= 1.0f);
 
    math::Quaternionf quat  = math::slerp(frames[frame].rotation, frames[frame+1].rotation, value);
    math::Vector3f    trans = math::lerp(frames[frame].translation, frames[frame+1].translation, value);

    return math::to_matrix_4x4(quat, trans);
}

math::Matrix4f AnimationTrack::getTransform(unsigned frame) const
{
    assert(frame < numFrames);
    return math::to_matrix_4x4(frames[frame].rotation, frames[frame].translation);
}

float AnimationTrack::getTime(unsigned frame) const
{
    return frames[frame].time;
}

unsigned AnimationTrack::getNumFrames() const
{
    return numFrames;
}

} // namespace detail
} // namespace animation
} // namespace slon