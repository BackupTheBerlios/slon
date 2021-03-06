#ifndef SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H
#define SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H

#include <sgl/Math/AABB.hpp>
#include <stack>
#include <vector>
#include "VisitorImpl.hpp"

namespace slon {
namespace scene {

/** Visits transformation nodes, performing worldToLocal, localToWorld matrices, AABB calculation. */
class TransformVisitor :
    public Visitor
{
private:
    struct traverse_node
    {
        explicit traverse_node(Node* node_ = 0) 
        :   node(node_)
        ,   transform(0)
        ,   dirty(false)
        {}

        Node*       node;
        Transform*  transform;
        bool        dirty;
    };

public:
    TransformVisitor();
    explicit TransformVisitor(Node& node);

    // Override NodeVisitor
    void traverse(Node& node);

    /** Get current local to world transform */
    const math::Matrix4f& getLocalToWorldTransform() const;

    /** Get current world to local transform */
    const math::Matrix4f& getWorldToLocalTransform() const;

    /** Get merged bounds of the traversed nodes */
    const math::AABBf& getBounds() const { return aabb; }

private:
    /** Add group children to the traverse queue. */
	void visitGroup(Group& group);

    /** Calculate worldToLocal and localToWorld transformations.
     * @return true if transform dirty.
     */
	bool visitTransform(Transform* parentTransform, Transform& transform);

    /** Extend AABB. */
    void visitEntity(Transform* parentTransform, Entity& entity);

	/** Compute skeleton children transformations in skeleton local space. */
	void visitSkeleton(Skeleton& skeleton);

	/** Compute skinning matrix. */
	void visitJoint(Transform* parentTransform, Joint& joint);

private:
    math::AABBf aabb;
    Transform*  currentTransform;
};

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_TRAVERSE_VISITOR_H
