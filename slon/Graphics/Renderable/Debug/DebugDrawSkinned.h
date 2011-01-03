#ifndef __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_DEBUG_DRAW_SKINNED_H__
#define __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_DEBUG_DRAW_SKINNED_H__

#include "../DebugMesh.h"

namespace slon {

// forward
namespace scene
{
    class Joint;
    class Skeleton;

    typedef boost::intrusive_ptr<const Joint>       const_joint_ptr;
    typedef boost::intrusive_ptr<const Skeleton>    const_skeleton_ptr;
}

namespace graphics {
namespace debug {

/** Debug primitive - joint. Actually draws bone consisting of this joint and its parent. */
struct joint
{
    joint(const scene::Joint*   joint_,
          float                 radius_ = -1.0f,
          float                 radiusFromBoneLength_ = 0.1f,
          bool                  drawChildren_ = true)
    :   debugJoint(joint_)
    ,   radius(radius_)
    ,   radiusFromBoneLength(radiusFromBoneLength_)
    ,   drawChildren(drawChildren_)
    {
        assert(debugJoint);
    }

    scene::const_joint_ptr  debugJoint;
    float                   radius;
    float                   radiusFromBoneLength;
    bool                    drawChildren;
};

/** Debug primitive - skeleton */
struct skeleton
{
    skeleton(const scene::Skeleton* skeleton_,
             float                  radius_ = -1.0f,
             float                  radiusFromBoneLength_ = 0.1f)
    :   debugSkeleton(skeleton_)
    ,   radius(radius_)
    ,   radiusFromBoneLength(radiusFromBoneLength_)
    {
        assert(debugSkeleton);
    }

    scene::const_skeleton_ptr   debugSkeleton;
    float                       radius;
    float                       radiusFromBoneLength;
};

/** Add joint to debug mesh. Assumes scene has been traversed. */
DebugMesh& operator << (DebugMesh& mesh, const joint& j);

/** Add skeleton to debug mesh */
DebugMesh& operator << (DebugMesh& mesh, const skeleton& s);

} // namespace debug
} // namespace graphics
} // namespace slon

#endif // __SLON_ENGINE_GRAPHICS_RENDERABLE_DEBUG_DEBUG_DRAW_SKINNED_H__4
