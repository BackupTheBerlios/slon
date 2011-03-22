#ifndef __SLON_ENGINE_SCENE_SKELETON_H__
#define __SLON_ENGINE_SCENE_SKELETON_H__

#include "Joint.h"

namespace slon {
namespace scene {

/** Skeleton represents hierarchy of the bones. Skeleton passes all visitors except
 * CullVisitor to the root joint, so adding geodes or other nodes affected
 * by CullVisitor in the skeletons joint hierarchy is meaningless.
 */
class Skeleton :
    public Node
{
public:
    typedef Node        base_type;
    typedef Skeleton    this_type;

public:
    /** Setup root bone of the skeleton. Setup root joint for the skeleton
     * only after you prepared joint hierarchy. Once you change it, resetup
     * root joint, it will force updating of the joint bind matrices.
     */
    virtual void setRootJoint(Joint* _root);

    /** Get root bone of the skeleton. */
    virtual Joint* getRootJoint() { return root.get(); }

    /** Get root bone of the skeleton. */
    virtual const Joint* getRootJoint() const { return root.get(); }

protected:
    joint_ptr root;
};

typedef boost::intrusive_ptr<Skeleton>          skeleton_ptr;
typedef boost::intrusive_ptr<const Skeleton>    const_skeleton_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_SKELETON_H__
