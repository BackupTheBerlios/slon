#ifndef __SLON_ENGINE_SCENE_GRAPH_GROUP_H__
#define __SLON_ENGINE_SCENE_GRAPH_GROUP_H__

#include "Node.h"
#include <boost/intrusive_ptr.hpp>

namespace slon {
namespace scene {

/** Group node stores links to the child nodes */
class Group :
    public Node
{
public:
    /** Get type of the node */
    TYPE getNodeType() const { return GROUP; }
    void accept(log::LogVisitor& visitor) const;
    
    /** Add child to the group.
     * @param child - new subnode of this group. If node has parent it will be removed from it.
	 * @param left - left node for the child.
     */
    void addChild(Node* child, Node* left = 0);

    /** Remove child from the group. Parent of this child must be this node. */
    void removeChild(Node* child);
	
    /** Remove all children of the group. */
    void removeChildren();
	
	/** Get first child of the group. */
	Node* getChild() { return firstChild.get(); }

	/** Get first child of the group. */
	const Node* getChild() const { return firstChild.get(); }

    virtual ~Group();

protected:
    node_ptr firstChild;
};

/** Find node in the scene graph using predicate
 * @param root - root of the subgraph to search for node
 * @param predicate - predicate for searching node
 * @return pointer to node if found, NULL otherwise
 */
template<typename T>
Node* findNode(Node& root, const T& predicate)
{
    if ( predicate(root) ) {
        return &root;
    }

    if ( Group* group = dynamic_cast<Group*>(&root) )
    {
        for(Node* i = group->getChild(); i; i = i->getRight())
        {
            if ( Node* node = findNode(*i, predicate) ) {
                return node;
            }
	    }
    }

    return 0;
}

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_GRAPH_GROUP_H__
