#include "stdafx.h"
#include "Database/Archive.h"
#include "Realm/BVHLocation.h"
#include "Realm/BVHLocationNode.h"

namespace slon {
namespace realm {

BVHLocationNode::BVHLocationNode(BVHLocation*      location_, 
                                 object_tree_node* node_, 
                                 bool              dynamic_)
:   location(location_)
,   node(node_)
,   dynamic(dynamic_)
{
}

const char* BVHLocationNode::serialize(database::OArchive& ar) const
{
    ar.writeSerializable(location);
    ar.writeChunk("dynamic", &dynamic);
    return "BVHLocationNode";
}

void BVHLocationNode::deserialize(database::IArchive& ar)
{
    location = ar.readSerializable<BVHLocation>();
    ar.readChunk<bool>("dynamic", &dynamic);
}

void BVHLocationNode::onUpdate()
{
    if (location) {
        location->update(firstChild);
    }
}

} // namespace realm
} // namespace slon