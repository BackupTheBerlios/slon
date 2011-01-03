#ifndef SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H
#define SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H

#include "NodeVisitor.h"
#include <sgl/Math/AABB.hpp>

namespace slon {
namespace scene {

/** Visitor pattern. Visits nodes, performing worldToLocal and
 * localToWorld matrices recomputation
 */
class TraverseVisitor :
    public NodeVisitor
{
protected:
    typedef std::vector<const Transform*>   const_transform_vector;
    typedef std::vector<size_t>             uint_vector;

public:
    TraverseVisitor();
    virtual ~TraverseVisitor() {}

    /** Get current local to world transform */
    virtual const math::Matrix4f& getLocalToWorldTransform() const;

    /** Get current world to local transform */
    virtual const math::Matrix4f& getWorldToLocalTransform() const;

    /** Get merged bounds of the traversed nodes */
    virtual const math::AABBf& getBounds() const { return aabb; }

    // Override NodeVisitor
    virtual void acceptBy(Node& node) { node.accept(*this); }
    virtual void traverse(Node& node);
	virtual void visitGroup(Group& group);
	virtual void visitTransform(Transform& transform);
    virtual void visitEntity(Entity& entity);

	/** Visit transform node, but do not change it. Use it if transformation node already
	 * knows its localToWorld and worldToLocal transforms.
	 */
	virtual void visitAbsoluteTransform(Transform& transform);

	/** Visit skeleton node and traverse node hierarchy. */
	virtual void visitSkeleton(Skeleton& skeleton);

	/** Visit joint node. */
	virtual void visitJoint(Joint& joint);

protected:
    // traverse
    mutable bool                    changeTransforms;
    mutable math::AABBf             aabb;
    mutable uint_vector             transformDepthStack;
	mutable const_transform_vector  transformNodeStack;
};

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H
