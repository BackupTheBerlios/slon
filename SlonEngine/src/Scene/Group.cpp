#include "stdafx.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/Group.h"

namespace slon {
namespace scene {

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