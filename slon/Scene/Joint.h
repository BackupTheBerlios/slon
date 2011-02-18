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
friend class TransformVisitor;
public:
    Joint();

    // unhide node functions
    using MatrixTransform::setTransform;
    using MatrixTransform::setInverseTransform;

    // Override Node
    TYPE getNodeType() const { return JOINT; }

    /** Set index of the joint in the skinned mesh joint array */
    void setId(unsigned id_) { id = id_; }

    /** Get index of the joint in the skinned mesh joint array */
    unsigned getId() const { return id; }

    /** Set transform */
    void setTransform(const math::Quaternionf& rotation, const math::Vector3f& translation);

    /** Set local to world transformation matrix */
    void setInverseTransform(const math::Quaternionf& rotation, const math::Vector3f& translation);

    /** Get inverse bind matrix to transform vertex into the joint coordinate system. */
    const math::Matrix4f& getInverseBindMatrix() const { return inverseBindMatrix; }

    /** Set inverse bind matrix to transform vertex into the joint coordinate system. */
    void setInverseBindMatrix(const math::Matrix4f& inverseBindMatrix_) { inverseBindMatrix = inverseBindMatrix_; }

    /** Get matrix used for skinning: localToWorld * objectToLocal */
    const math::Matrix4f& getSkinningMatrix() const { return skinningMatrix; }

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
