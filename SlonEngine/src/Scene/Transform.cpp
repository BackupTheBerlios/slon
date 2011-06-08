#include "stdafx.h"
#include "Database/Archive.h"
#include "Detail/Engine.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Realm/World.h"
#include "Scene/Transform.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("scene.Group");

using namespace slon;
using namespace scene;

Transform::Transform(const hash_string& name)
:	Group(name)
,	traverseStamp(0)
,	modifiedCount(0)
{
}
	
// Override Serializable
const char* Transform::getSerializableName() const
{
	return "scene::Transform";
}

void Transform::serialize(database::OArchive& ar) const
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }
	
	// serialize base class
	Group::serialize(ar);

	// serialize data
	ar.writeChunk("worldToLocal", worldToLocal.data(), worldToLocal.num_elements);
	ar.writeChunk("localToWorld", localToWorld.data(), localToWorld.num_elements);
	ar.writeChunk("traverseStamp", &traverseStamp);
	ar.writeChunk("modifiedCount", &modifiedCount);
}

void Transform::deserialize(database::IArchive& ar)
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

	// deserialize base class
	Group::deserialize(ar);

	// deserialize data
	readChunk(ar, "worldToLocal", worldToLocal.data(), worldToLocal.num_elements);
	readChunk(ar, "localToWorld", localToWorld.data(), localToWorld.num_elements);
	readChunk(ar, "traverseStamp", &traverseStamp);
	readChunk(ar, "modifiedCount", &modifiedCount);
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