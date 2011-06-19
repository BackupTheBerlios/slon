#ifndef __SLON_ENGINE_DATABASE_DETAIL_UTILITY_SERIALIZATION_H__
#define __SLON_ENGINE_DATABASE_DETAIL_UTILITY_SERIALIZATION_H__

#include "../../Utility/Algorithm/aabb_tree.hpp"
#include "SGLSerialization.h"

namespace slon {
namespace database {

// math serialization
template<typename LeafData, typename RealType, typename Func>
void serialize(OArchive& ar, const char* name, const aabb_tree<LeafData, RealType>& tree, Func leafSerializer)
{
    typedef aabb_tree<LeafData, RealType>::volume_node volume_node;

    ar.openChunk(name);
    const volume_node* node = tree.get_root();
    while (node)
    {
        ar.openChunk("volume_node");
        serialize(ar, "volume", node->get_bounds());

        bool leaf = node->is_leaf();
        ar.writeChunk("leaf", &leaf);
        if (leaf) 
        {
            leafSerializer(ar, node->as_leaf()->data);
            ar.closeChunk();

            // go up until we find nonvisited right child
            const volume_node* parent = node->get_parent();
            while (parent && parent->get_child(1) == node) 
            { 
                node   = parent;
                parent = node->get_parent();
                ar.closeChunk();
            }

            if (parent) {
                node = parent->get_child(1);
            }
        }
        else {
            node = node->get_child(0);
        }
    }
    ar.closeChunk();
}

template<typename LeafData, typename RealType, typename Func>
typename aabb_tree<LeafData, RealType>::volume_node* deserialize_aabb_tree_node(IArchive& ar, Func leafDeserializer)
{
    typedef aabb_tree<LeafData, RealType>::volume_node volume_node;
    typedef aabb_tree<LeafData, RealType>::leaf_node   leaf_node;
    typedef aabb_tree<LeafData, RealType>::aabb_type   aabb_type;

    volume_node* node = 0;
    aabb_type    volume;
    deserialize(ar, "volume", volume);

    bool leaf;
    ar.readChunk("leaf", &leaf);
    if (leaf) {
        node = new leaf_node(volume, leafDeserializer(ar));
    }
    else 
    {
        IArchive::chunk_info info;
        node = new volume_node(volume);
        ar.openChunk("volume_node", info);
        node->set_child( 0, deserialize_aabb_tree_node<LeafData, RealType>(ar, leafDeserializer) );
        ar.closeChunk();
        ar.openChunk("volume_node", info);
        node->set_child( 1, deserialize_aabb_tree_node<LeafData, RealType>(ar, leafDeserializer) );
        ar.closeChunk();
    }

    return node;
}

template<typename LeafData, typename RealType, typename Func>
void deserialize(IArchive& ar, const char* name, aabb_tree<LeafData, RealType>& tree, Func leafDeserializer)
{
    IArchive::chunk_info info;
    if ( !ar.openChunk(name, info) ) {
        throw serialization_error("Can't open AABB tree chunk");
    }

    aabb_tree<LeafData, RealType>::volume_node* root = 0;
    if ( ar.openChunk("volume_node", info) )
    {
        root = deserialize_aabb_tree_node<LeafData, RealType>(ar, leafDeserializer);
        ar.closeChunk();
    }

    tree.set_root(root);
    ar.closeChunk();
}

} // namespace slon
} // namespace database

#endif // __SLON_ENGINE_DATABASE_DETAIL_UTILITY_SERIALIZATION_H__