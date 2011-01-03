#ifndef SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H
#define SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H

#include "Transform.h"

namespace slon {
namespace scene {

/** Matrix transform class allows to insert transformation matrix into the scene graph.
 * worldToLocal, localToWorld and local inverted transformation matrices are cached.
 * They update during the traversals.
 */
class MatrixTransform :
    public Transform
{
public:
    MatrixTransform();

    // unhide transform functions
    using Transform::getTransform;
    using Transform::getInverseTransform;

    // Override Transform
    const math::Matrix4f&   getTransform() const;
    const math::Matrix4f&   getInverseTransform() const;
    unsigned int            getModifiedTS() const { return modifiedCount; }

    /** Set world to local transformation matrix */
    virtual void setTransform(const math::Matrix4f& matrix);

    /** Set local to world transformation matrix */
    virtual void setInverseTransform(const math::Matrix4f& matrix);

    /** Set both transform and inverse transform, neither checks performed */
    virtual void setTransformAndInverse(const math::Matrix4f& matrix, const math::Matrix4f& invMatrix);

    virtual ~MatrixTransform() {}

protected:
    // transform
    math::Matrix4f transform;
    math::Matrix4f invTransform;
    unsigned int   modifiedCount;
    mutable bool   transformDirty;
    mutable bool   invTransformDirty;
};

typedef boost::intrusive_ptr<MatrixTransform>       matrix_transform_ptr;
typedef boost::intrusive_ptr<const MatrixTransform> const_matrix_transform_ptr;

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H
