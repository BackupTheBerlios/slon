#ifndef SLON_ENGINE_SCENE_GRAPH_NODE_VISITOR_H
#define SLON_ENGINE_SCENE_GRAPH_NODE_VISITOR_H

#include "../Entity.h"
#include "../Skeleton.h"
#include <limits>
#include <vector>

namespace slon {
namespace scene {

/** Visitor traverses scene graph and operates on different kind of nodes */
class NodeVisitor
{
public:
    typedef std::vector<Node*>  node_vector;

public:
    NodeVisitor() :
        visitMask( std::numeric_limits<Node::mask_type>::max() )
    {}

    /** Override this function and call: 
     * \code
     * node.accept(*this); 
     * \uncode
     * This will call accept function using your visitor type.
     */ 
    virtual void acceptBy(Node& node) { node.accept(*this); }

    /** Traditional DFS tree traverse. Override if you mind to change it. */
    virtual void traverse(Node& node)
    {
        forTraverse.push_back(&node);
        while ( !forTraverse.empty() )
        {
            Node* traversed = forTraverse.back(); forTraverse.pop_back();
            acceptBy(*traversed);
        }
    }

	virtual void visitNode(Node& /*node*/) {}

	virtual void visitGroup(Group& group)
    {
        for( Group::node_iterator i = group.firstChild();
                                  i != group.endChild();
                                  ++i )
        {
            if ( ((*i)->getAcceptMask() & visitMask) != 0 ) {
                forTraverse.push_back( (*i).get() );
            }
        }
    }

	virtual void visitTransform(Transform& transform)
    {
        if ( (transform.getAcceptMask() & visitMask) == 0 ) {
            return;
        }

        visitGroup(transform);
    }

    virtual void visitEntity(Entity& /*entity*/) {}

    /** Prevent further traverse */
    virtual void breakTraverse()
    {
        forTraverse.clear();
    }

    /** Setup visit mask for the visitor. 
     * @see scene::Node::setAcceptMask
     */
    virtual void setVisitMask(Node::mask_type _visitMask) { visitMask = _visitMask; }

    /** Get visit mask for the visitor. 
     * @see scene::Node::getAcceptMask
     */
    virtual Node::mask_type getVisitMask() const { return visitMask; }

    virtual ~NodeVisitor() {}

protected:
    Node::mask_type     visitMask;

    // traverse
    mutable node_vector forTraverse;
};

} // namepsace sg
} // namespace pine

#endif // SLON_ENGINE_SCENE_GRAPH_NODE_VISITOR_H
