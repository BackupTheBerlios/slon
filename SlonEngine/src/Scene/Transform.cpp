#include "stdafx.h"
#include "Detail/Engine.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Realm/World.h"
#include "Scene/Transform.h"

using namespace slon;
using namespace scene;

Transform::Transform() :
    traverseStamp(0),
	modifiedCount(0)
{
}

void Transform::accept(log::LogVisitor& visitor) const
{
    visitor << "Transform";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "transform =" << log::detailed(getTransform(), true)  
            << "localToWorld =" << log::detailed(getLocalToWorld(), true) ; 
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}

void Transform::update()
{
	++modifiedCount;

	Node* node = this;
	while (node && !node->getObject()) {
		node = node->getParent();
	}

	if ( node && node->getObject() ) {
		detail::Engine::engineInstance->addToUpdateQueue(node->getObject());
	}
}