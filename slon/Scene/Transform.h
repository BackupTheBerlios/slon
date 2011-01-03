#ifndef SLON_ENGINE_SCENE_GRAPH_TRANSFORM_H
#define SLON_ENGINE_SCENE_GRAPH_TRANSFORM_H

#include "Group.h"
#include <sgl/Math/Matrix.hpp>

namespace slon {
namespace scene {

/** All childs of the Transform node are in local space */
class Transform :
    public Group
{
friend class TraverseVisitor;
public:
    Transform();

    // Override Node
    void accept(NodeVisitor& visitor);
    void accept(TraverseVisitor& visitor);
    void accept(UpdateVisitor& visitor);
    void accept(CullVisitor& visitor);

    /** Get type of the node */
    virtual TYPE getType() const { return TRANSFORM; }

    /** Get node transformation matrix
     * @return node transformation matrix
     */
    virtual const math::Matrix4f& getTransform() const = 0;

    /** Get node inverse transformation matrix
     * @return matrix that transforms node from world space to local space
     */
    virtual const math::Matrix4f& getInverseTransform() const = 0;

    /** Get last modification time stamp. */
    virtual unsigned int getModifiedTS() const = 0;

    /** Get last traverse time stamp. */
    virtual unsigned int getTraverseTS() const { return traverseTS; }

    /** Get world to local space transformation matrix
     * @return cached transform from last traverse.
     *  If no traverse were called result is undefined
     */
    virtual const math::Matrix4f& getWorldToLocal() const { return worldToLocal; }

    /** Get local to world space transformation matrix
     * @return cached transform from last traverse
     *  If no traverse were called result is undefined
     */
    virtual const math::Matrix4f& getLocalToWorld() const { return localToWorld; }

    virtual ~Transform() {}

protected:    
    // cached transforms
    math::Matrix4f  worldToLocal;
    math::Matrix4f  localToWorld;
    unsigned int    traverseTS;
};

typedef boost::intrusive_ptr<Transform>         transform_ptr;
typedef boost::intrusive_ptr<const Transform>   const_transform_ptr;

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_TRANSFORM_H
