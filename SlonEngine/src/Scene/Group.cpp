#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/Group.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("scene.Group");

namespace slon {
namespace scene {
	
Group::Group()
{
}

Group::Group(const hash_string& name)
:	Node(name)
{
}

void Group::serialize(database::OArchive& ar) const
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }
	
	// serialize base class
	Node::serialize(ar);

	// serialize children
	node_ptr child(firstChild);
	if (child)
	{
		ar.openChunk("children");
		while (child) 
		{
			ar.writeSerializable( child.get() );
			child = child->getRight();
		}
		ar.closeChunk();
	}

	return "Group";
}

void Group::deserialize(database::IArchive& ar)
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

	// deserialize base class
	Node::deserialize(ar);

	// deserialize children
	database::IArchive::chunk_info info;
	if ( ar.openChunk("children", info) )
	{
		scene::Node* left = 0;
		while ( Serializable* s = ar.readSerializable() ) 
		{
			scene::Node* node = dynamic_cast<scene::Node*>(s);
			if (!node) {
				throw database::serialization_error(AUTO_LOGGER, "Deserialized scene::Group child is not convertible to scene::Node.");
			}
			addChild(node, left);
			left = node;
		}
		ar.closeChunk();
	}
}

void Group::addChild(Node* child, Node* left)
{
	assert(child->parent != this && (!left || left->parent == this));
	node_ptr guard(child);

	if (child->parent) {
		child->parent->removeChild(child);
	}

	child->parent = this;
	if (!left) 
	{
		child->left = 0;
		child->right = firstChild.get();
		if (firstChild) {
			firstChild->left = child;
		}
		firstChild = child;
	}
	else 
	{
		child->right = left->right;
		left->right = child;
		child->left = left;
	}
}

void Group::removeChild(Node* child)
{
	assert(child->parent == this);
	node_ptr guard(child);

	if (child->left) {
		child->left->right = child->right;
	}
	if (child == firstChild) {
		firstChild = child->right;
	}
	if (child->right) {
		child->right->left = child->left;
	}
	child->parent = 0;
	child->right = 0;
	child->left = 0;
}

void Group::removeChildren()
{
	while (firstChild != 0)
	{
		node_ptr next(firstChild->right);
		firstChild->parent = 0;
		firstChild->left = 0;
		firstChild->right = 0;
		firstChild = next;
	}
}

Group::~Group()
{
	removeChildren();
}

void Group::accept(log::LogVisitor& visitor) const
{    
    visitor << "Group";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent();
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}

} // namespace scene
} // namespace slon