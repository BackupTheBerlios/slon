#include "stdafx.h"
#include "Database/Archive.h"
#include "Detail/Engine.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Scene/Transform.h"
#include "Utility/error.hpp"

DECLARE_AUTO_LOGGER("scene.Transform");

using namespace slon;
using namespace scene;

Transform::Transform(const hash_string& name)
:	Group(name)
,	traverseStamp(0)
,	modifiedCount(1)
{
}
	
// Override Serializable
const char* Transform::serialize(database::OArchive& ar) const
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

    return "Transform";
}

void Transform::deserialize(database::IArchive& ar)
{
    if ( ar.getVersion() < database::getVersion(0, 1, 0) ) {
        throw database::serialization_error(AUTO_LOGGER, "Trying to serialize using unsupported version");
    }

	// deserialize base class
	Group::deserialize(ar);

	// deserialize data
	ar.readChunk("worldToLocal", worldToLocal.data(), worldToLocal.num_elements);
	ar.readChunk("localToWorld", localToWorld.data(), localToWorld.num_elements);
	ar.readChunk("traverseStamp", &traverseStamp);
	ar.readChunk("modifiedCount", &modifiedCount);
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
    doUpdate();
}