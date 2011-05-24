#ifndef __SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H__
#define __SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H__

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
    MatrixTransform(const math::Matrix4f& transform);

    // Override Node
    void accept(log::LogVisitor& visitor) const;

    // unhide transform functions
    using Transform::getTransform;
    using Transform::getInverseTransform;

    // Override Transform
    bool                    isAbsolute() const { return false; }
    const math::Matrix4f&   getTransform() const;
    const math::Matrix4f&   getInverseTransform() const;

    /** Set world to local transformation matrix */
    void setTransform(const math::Matrix4f& matrix);

    /** Set local to world transformation matrix */
    void setInverseTransform(const math::Matrix4f& matrix);

    /** Set both transform and inverse transform, neither checks performed */
    void setTransformAndInverse(const math::Matrix4f& matrix, const math::Matrix4f& invMatrix);
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<Group>(*this);
        ar & BOOST_SERIALIZATION_NVP(transform);
        invTransformDirty = true;
    }

    virtual ~MatrixTransform() {}

protected:
    // transform
    math::Matrix4f transform;
    math::Matrix4f invTransform;
    mutable bool   transformDirty;
    mutable bool   invTransformDirty;
};

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_GRAPH_MATRIX_TRANSFORM_H__
