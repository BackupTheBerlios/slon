#ifndef __SLON_ENGINE_REALM_LOCATION_NODE_H__
#define __SLON_ENGINE_REALM_LOCATION_NODE_H__

#include "../Scene/Group.h"
#include "../Utility/Algorithm/aabb_tree.hpp"
#include "Forward.h"

namespace slon {
namespace realm {

/** BVHLocationNode stores information necessary for BVHLocation class. 
 * Before inserting node in spatial structure BVHLocation will add this node as parent 
 * for the scene graph and will store necessary information in it.
 * @see BVHLocation
 */
class SLON_PUBLIC BVHLocationNode :
    public scene::Group
{
public:
    typedef aabb_tree<bvh_location_node_ptr>    object_tree;
    typedef object_tree::leaf_node              object_tree_node;
    typedef object_tree::iterator               object_tree_iterator;

public:
    BVHLocationNode(BVHLocation*      location_ = 0, 
                    object_tree_node* node_     = 0, 
                    bool              dynamic_  = true);
    ~BVHLocationNode();

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    /** Get location where this node belongs */
    BVHLocation* getLocation() { return location; }

    /** Get location where this node belongs */
    const BVHLocation* getLocation() const { return location; }

    /** Set node locating node in AABB tree. */
    void setBVHNode(object_tree_node* node_) { node = node_; }

    /** Set iterator locating node in AABB tree. */
    void setBVHIterator(const object_tree_iterator& iter) { node = iter.get_node(); }

    /** Get node locating node in AABB tree.  */
    object_tree_node* getBVHNode() const { return node; }

    /** Get iterator locating node in AABB tree. */
    object_tree_iterator getBVHIterator() const { return object_tree_iterator(node); }

    /** Check whether object frequently transforms (hint). */
    bool isDynamic() const { return dynamic; }

    /** Set hint specifying that object is frequently updated. */
    void toggleDynamic(bool dynamic_) { dynamic = dynamic_; }

    // Override Node
    void onUpdate();

private:
    BVHLocation*        location;
    object_tree_node*   node;
    bool                dynamic;
};

} // namespace realm
} // namespace slon

#endif // __SLON_ENGINE_REALM_LOCATION_NODE_H__
