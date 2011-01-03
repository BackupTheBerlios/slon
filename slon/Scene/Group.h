#ifndef SLON_ENGINE_SCENE_GRAPH_GROUP_H
#define SLON_ENGINE_SCENE_GRAPH_GROUP_H

#include "Node.h"
#include <vector>
#include <cassert>

namespace slon {
namespace scene {

/** Group node stores links to the child nodes */
class Group :
    public Node
{
public:
    typedef std::vector<node_ptr> 		node_vector;
    typedef node_vector::iterator       node_iterator;
    typedef node_vector::const_iterator const_node_iterator;

public:
    /** Get type of the node */
    virtual TYPE getType() const { return GROUP; }

    /** Find child in the group
     * @param child - node that we are searching
     * @return iterator addresing child in the child array
     */
    virtual const_node_iterator findChild(Node& child) const;

    /** Find child in the group
     * @param child - node that we are searching
     * @return iterator addresing child in the child array
     */
    virtual node_iterator findChild(Node& child);
    
    /** Add child to the specified group. Remove it from this group.
     * @param group - group where to add child.
     * @param child - child to move to another group.
     */
    virtual void moveChild(Group& group, Node& child);

    /** Add child to the specified group. Remove it from this group. 
     * @param group - group where to add child.
     * @param childIter - iterator addressing child to move to another group.
     */
    virtual void moveChild(Group& group, const node_iterator& childIter);

    /** Add child to the group.
     * @param child - new group child. It may not to have parent node.
     */
    virtual void addChild(Node& child);

    /** Remove child from the group. Parent of this child must be this node.
     * @param child - child to remove
     */
    virtual void removeChild(Node& child);

    /** Remove child from the group
     * @param childIter - iterator addresing child for removal
     * @return true if child succesfully removed
     */
    virtual void removeChild(const node_iterator& childIter);

    /** Replace child in from the group
     * @param child - child to remove. Parent of this child must be this node.
     * @param child - child to insert
     */
    virtual void replaceChild( Node& child,
                               Node& newChild );

    /** Replace child in from the group
     * @param childIter - iterator addresing child to remove
     * @param child - child to insert
     * @return true if child found and replaced
     */
    virtual void replaceChild( const node_iterator& childIter,
                               Node& newChild );

    /** Remove all children of the group */
    virtual void removeAllChildren() { children.clear(); }

    /** Get number of the node children
     * @return node children number
     */
    virtual size_t getNumChildren() const { return children.size(); }

    /** Get i'th child node
     * @param index of the child
     * @return i'th child node
     */
    virtual Node* getChild(size_t i) const
    {
        assert( i < children.size() );
        return children[i].get();
    }

    /** Get iterator addressing first child of the Group */
    virtual node_iterator firstChild() { return children.begin(); }

    /** Get iterator addressing first child of the Group */
    virtual const_node_iterator firstChild() const { return children.begin(); }

    /** Get iterator addressing child after last child of the group */
    virtual node_iterator endChild() { return children.end(); }

    /** Get iterator addressing child after last child of the group */
    virtual const_node_iterator endChild() const { return children.end(); }

    /** Accept NodeVisitor */
    virtual void accept(NodeVisitor& visitor);

    /** Accept TraverseVisitor */
    virtual void accept(TraverseVisitor& visitor);

    /** Accept UpdateVisitor */
    virtual void accept(UpdateVisitor& visitor);

    /** Accept CullVisitor */
    virtual void accept(CullVisitor& visitor);

    virtual ~Group();

protected:
    node_vector children;
};

typedef boost::intrusive_ptr<Group>             group_ptr;
typedef boost::intrusive_ptr<const Group>       const_group_ptr;

/** Find node in the scene graph using predicate
 * @param root - root of the subgraph to search for node
 * @param predicate - predicate for searching node
 * @return pointer to node if found, NULL otherwise
 */
template<typename T>
Node* findNode( Node&       root,
                const T&    predicate )
{
    if ( predicate(root) ) {
        return &root;
    }

    if ( Group* group = dynamic_cast<Group*>(&root) )
    {
        for( Group::node_iterator i = group->firstChild();
                                  i != group->endChild();
                                  ++i )
        {
		    Node* node = findNode(**i, predicate);
            if (node) {
                return node;
            }
	    }
    }

    // empty node
    return 0;
}

} // namespace scene
} // namespace slon

#endif // SLON_ENGINE_SCENE_GRAPH_GROUP_H
