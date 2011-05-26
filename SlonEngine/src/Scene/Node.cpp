#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/LogVisitor.h"
#include "Scene/Group.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("scene.Node");

namespace slon {
namespace scene {

Node::Node() 
:   parent(0)
,	left(0)
,	right(0)
,	userPointer(0)
,	object(0)
{}

Node::Node(hash_string name_)
:	name(name_)
,	parent(0)
,	left(0)
,	right(0)
,	userPointer(0)
,	object(0)
{
}

void Node::accept(log::LogVisitor& visitor) const 
{
    if ( visitor.getLogger() )
    {
        visitor << "Node";
        if ( getName() != "" ) {
            visitor << " '" << getName() << "'";
        }
        visitor << " {}\n";
    }
}

const char* Node::getSerializableName() const    
{ 
    return "scene::Node";
}

void Node::serialize(database::OArchive& ar) const
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

    ar.writeStringChunk( "name", name.str().data(), name.str().length() );
}

void Node::deserialize(database::IArchive& ar)
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

	name = hash_string( database::readStringChunk(ar, "name") );
}

Node* findNamedNode(Node& root, hash_string name)
{
    if ( root.getName() == name ) {
        return &root;
    }

    if ( Group* group = dynamic_cast<Group*>(&root) )
    {
        for(Node* i = group->getChild(); i; i = i->getRight())
        {
            if ( Node* node = findNamedNode(*i, name) ) {
                return node;
            }
	    }
    }

    // empty node
    return 0;
}

} // namespace slon
} // namespace scene
