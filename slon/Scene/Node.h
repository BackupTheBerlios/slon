#ifndef SLON_ENGINE_SCENE_GRAPH_NODE_H
#define SLON_ENGINE_SCENE_GRAPH_NODE_H

#include "../Utility/callback.hpp"
#include <boost/intrusive_ptr.hpp>
#include <limits>
#include <sgl/Utility/Aligned.h>
#include <string>

namespace slon {
namespace scene {

// forward
class Group;

class NodeVisitor;
class TraverseVisitor;
class UpdateVisitor;
class CullVisitor;

/** Tree Node class. Stores hierarchy. Use intrusive ptr
 * on scene graph structures. You can't construct nodes on the stack.
 */
class Node :
    public Referenced
{
friend class Group;
public:
    /// Type of the scene graph node
    enum TYPE
    {
        NODE,
        GROUP,
        TRANSFORM,
        ENTITY
    };

    typedef unsigned int    mask_type;

    typedef callback<void (Node&, NodeVisitor&)>       node_visitor_callback;
    typedef callback<void (Node&, TraverseVisitor&)>   traverse_visitor_callback;
    typedef callback<void (Node&, UpdateVisitor&)>     update_visitor_callback;
    typedef callback<void (Node&, CullVisitor&)>       cull_visitor_callback;

    typedef boost::intrusive_ptr<node_visitor_callback>       node_visitor_callback_ptr;
    typedef boost::intrusive_ptr<traverse_visitor_callback>   traverse_visitor_callback_ptr;
    typedef boost::intrusive_ptr<update_visitor_callback>     update_visitor_callback_ptr;
    typedef boost::intrusive_ptr<cull_visitor_callback>       cull_visitor_callback_ptr;

private:
    // noncopyable
    Node(const Node&);
    Node& operator = (const Node&);

public:
    /** Create empty node */
    Node() :
        parent(0),
        acceptMask( std::numeric_limits<mask_type>::max() )
    {}

    /** Create node with specified name*/
    explicit Node(const std::string& _name) :
        name(_name),
        parent(0),
        acceptMask( std::numeric_limits<mask_type>::max() )
    {}

    /** Get type of the node */
    virtual TYPE getType() const { return NODE; }

    /** Get parent node */
    Group* getParent() const { return parent; }

	/** Setup node name */
	virtual void setName(const std::string& _name) { name = _name; }

	/** Get node name */
	virtual const std::string& getName() const { return name; }

    /** Setup accept mask. Node visitor will visit this node if (node_mask & visitor_mask) != 0.
     * Mask test is handled by the visitor.
     */
    virtual void setAcceptMask(mask_type _acceptMask) { acceptMask = _acceptMask; }

    /** Get accept mask. Node visitor will visit this node if (node_mask & visitor_mask) != 0.
     * Mask test is handled by the visitor.
     */
    virtual mask_type getAcceptMask() const { return acceptMask; }

    /** Accept NodeVisitor */
    virtual void accept(NodeVisitor& visitor);

    /** Accept TraverseVisitor */
    virtual void accept(TraverseVisitor& visitor);

    /** Accept UpdateVisitor */
    virtual void accept(UpdateVisitor& visitor);

    /** Accept CullVisitor */
    virtual void accept(CullVisitor& visitor);

    /** Set callback for handling NodeVisitor */
    virtual void setNVCallback(node_visitor_callback* callback) { nvCallback.reset(callback); }

    /** Set callback for handling TraverseVisitor */
    virtual void setTVCallback(traverse_visitor_callback* callback) { tvCallback.reset(callback); }
    
    /** Set callback for handling UpdateVisitor */
    virtual void setUVCallback(update_visitor_callback* callback) { uvCallback.reset(callback); }
    
    /** Set callback for handling CullVisitor */
    virtual void setCVCallback(cull_visitor_callback* callback) { cvCallback.reset(callback); }

    /** Get callback for handling NodeVisitor */
    virtual node_visitor_callback* getNVCallback() { return nvCallback.get(); }

    /** Get callback for handling TraverseVisitor */
    virtual traverse_visitor_callback* getTVCallback() { return tvCallback.get(); }
    
    /** Get callback for handling UpdateVisitor */
    virtual update_visitor_callback* getUVCallback() { return uvCallback.get(); }
    
    /** Get callback for handling CullVisitor */
    virtual cull_visitor_callback* getCVCallback() { return cvCallback.get(); }

    /** Get callback for handling NodeVisitor */
    virtual const node_visitor_callback* getNVCallback() const { return nvCallback.get(); }

    /** Get callback for handling TraverseVisitor */
    virtual const traverse_visitor_callback* getTVCallback() const { return tvCallback.get(); }
    
    /** Get callback for handling UpdateVisitor */
    virtual const update_visitor_callback* getUVCallback() const { return uvCallback.get(); }
    
    /** Get callback for handling CullVisitor */
    virtual const cull_visitor_callback* getCVCallback() const { return cvCallback.get(); }

    virtual ~Node() {}

protected:
	// node info
	std::string name;
    Group*      parent;
    mask_type   acceptMask;

    node_visitor_callback_ptr       nvCallback;
    traverse_visitor_callback_ptr   tvCallback;
    update_visitor_callback_ptr     uvCallback;
    cull_visitor_callback_ptr       cvCallback;
};

typedef boost::intrusive_ptr<Node>              node_ptr;
typedef boost::intrusive_ptr<const Node>        const_node_ptr;

/** Find node with specified name in the graph
 * @param root - root of the subgraph to search for node
 * @param name - name of the node to search
 * @return pointer to node if found, NULL otherwise
 */
Node* findNamedNode( Node& root,
                     const std::string& name );

} // namespace scene
} // namepsace slon

#endif // SLON_ENGINE_SCENE_GRAPH_NODE_H
