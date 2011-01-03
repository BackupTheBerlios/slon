#ifndef __SLON_ENGINE_SCENE_JOINT_H__
#define __SLON_ENGINE_SCENE_JOINT_H__

#include "MatrixTransform.h"
#include <sgl/Math/Quaternion.hpp>

namespace slon {
namespace scene {

/** Joint class is transform class specialized for skinning.
 */
class Joint :
    public MatrixTransform
{
friend class TraverseVisitor;
public:
    Joint();

    // unhide node functions
    using Node::accept;
    using MatrixTransform::setTransform;
    using MatrixTransform::setInverseTransform;

    // Override node
    void accept(TraverseVisitor& visitor);

    /** Set index of the joint in the skinned mesh joint array */
    virtual void setId(unsigned id_) { id = id_; }

    /** Get index of the joint in the skinned mesh joint array */
    virtual unsigned getId() const { return id; }

    /** Set transform */
    virtual void setTransform(const math::Quaternionf& rotation, const math::Vector3f& translation);

    /** Set local to world transformation matrix */
    virtual void setInverseTransform(const math::Quaternionf& rotation, const math::Vector3f& translation);

    /** Get inverse bind matrix to transform vertex into the joint coordinate system. */
    virtual const math::Matrix4f& getInverseBindMatrix() const { return inverseBindMatrix; }

    /** Set inverse bind matrix to transform vertex into the joint coordinate system. */
    virtual void setInverseBindMatrix(const math::Matrix4f& inverseBindMatrix_) { inverseBindMatrix = inverseBindMatrix_; }

    /** Get matrix used for skinning: localToWorld * objectToLocal */
    virtual const math::Matrix4f& getSkinningMatrix() const { return skinningMatrix; }

    virtual ~Joint() {}

protected:
    math::Matrix4f  inverseBindMatrix;
    math::Matrix4f  skinningMatrix;
    unsigned        id;
};

// ptr typedef
typedef boost::intrusive_ptr<Joint>          joint_ptr;
typedef boost::intrusive_ptr<const Joint>    const_joint_ptr;

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_JOINT_H__
