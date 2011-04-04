#ifndef __SLON_ENGINE_SCENE_NODE_H__
#define __SLON_ENGINE_SCENE_NODE_H__

#include "../Log/Forward.h"
#include "../Realm/Forward.h"
#include "../Utility/hash_string.hpp"
#include "Forward.h"

namespace slon {
namespace scene {

/** Tree Node class. Stores hierarchy. Use intrusive ptr
 * on scene graph structures. You can't construct nodes on the stack.
 */
class Node :
    public Referenced
{
friend class Group;
public:
    enum
    {
        NODE_BIT      = 0,
        SKELETON_BIT  = 1,
        GROUP_BIT     = 1 << 1,
        TRANSFORM_BIT = 1 << 2,
        JOINT_BIT     = 1 << 3,
        ENTITY_BIT    = 1 << 4,
        LIGHT_BIT     = 1 << 5,
        CAMERA_BIT    = 1 << 6,
        GEODE_BIT     = 1 << 7
    };

    /// Type of the scene graph node
    enum TYPE
    {
        NODE      = 0,
        SKELETON  = SKELETON_BIT,
        GROUP     = GROUP_BIT,
        TRANSFORM = GROUP | TRANSFORM_BIT,
        JOINT     = TRANSFORM | JOINT_BIT,
        ENTITY    = ENTITY_BIT,
        LIGHT     = ENTITY | LIGHT_BIT,
        CAMERA    = ENTITY | CAMERA_BIT,
        GEODE     = ENTITY | GEODE_BIT
    };

private:
    // noncopyable
    Node(const Node&);
    Node& operator = (const Node&);

public:
    Node();
    explicit Node(hash_string name);

    /** Get type of the node */
    virtual TYPE getNodeType() const { return NODE; }

    /** Helper function outputs information about node. Override in derived classes. */
    virtual void accept(log::LogVisitor& visitor) const;

    /** Get parent node in the hierarchy. */
    Group* getParent() { return parent; }

    /** Get parent node in the hierarchy. */
    const Group* getParent() const { return parent; }

	/** Get left node in the hierarchy. */
	Node* getLeft() { return left; }

	/** Get left node in the hierarchy. */
	const Node* getLeft() const { return left; }

	/** Get right node in the hierarchy. */
	Node* getRight() { return right.get(); }

	/** Get right node in the hierarchy. */
	const Node* getRight() const { return right.get(); }

	/** Setup user data, use it on your own. */
	void setUserPointer(void* userPointer_) { userPointer = userPointer_; }
	
	/** Get user data. */
	const void* getUserPointer() const { return userPointer; }

	/** Get user data. */
	void* getUserPointer() { return userPointer; }

	/** Set node name */
	void setName(hash_string _name) { name = _name; }

	/** Get node name. */
	hash_string getName() const { return name; }

	/** Get object to which holds this node */
	realm::Object* getObject() { return object; }

	/** Set object which holds this node */
	void setObject(realm::Object* object_) { object = object_; }
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(name);
    }

    virtual ~Node() {}

protected:
	// node info
	hash_string     name;
    Group*          parent;
	Node*			left;
	node_ptr		right;
	void*			userPointer;

	realm::Object*	object;
};

/** Find node with specified name in the graph
 * @param root - root of the subgraph to search for node
 * @param name - name of the node to search
 * @return pointer to node if found, NULL otherwise
 */
Node* findNamedNode(Node& root, hash_string name);

} // namespace scene
} // namespace slon

#endif // __SLON_ENGINE_SCENE_NODE_H__
