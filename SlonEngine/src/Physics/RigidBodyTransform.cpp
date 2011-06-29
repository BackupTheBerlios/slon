#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/RigidBodyTransform.h"

namespace slon {
namespace physics {

RigidBodyTransform::RigidBodyTransform(const rigid_body_ptr& rigidBody_) :
    rigidBody(rigidBody_),
    absolute(false)
{
}

const char* RigidBodyTransform::serialize(database::OArchive& ar) const
{
	// serialize base class
	MatrixTransform::serialize(ar);

	// serialize data
    ar.writeSerializable(rigidBody.get());
    ar.writeChunk("absolute", &absolute);
    return "RigidBodyTransform";
}

void RigidBodyTransform::deserialize(database::IArchive& ar)
{
	// deserialize base class
	MatrixTransform::deserialize(ar);

	// deserialize data
    rigidBody = ar.readSerializable<physics::RigidBody>();
    ar.readChunk("absolute", &absolute);
}

void RigidBodyTransform::accept(log::LogVisitor& visitor) const
{
    visitor << "RigidBodyTransform";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "rigidBody =";
    if (rigidBody)
    {
        if ( rigidBody->getName() != "" ) {
            visitor << "'" << rigidBody->getName() << "'";
        }
        else {
            visitor << "unnamed(" << rigidBody << ")";
        }
    }
    else {
        visitor << "0";
    }
    visitor << "\n"
            << "transform =" << log::detailed(getTransform(), true)  
            << "localToWorld =" << log::detailed(getLocalToWorld(), true);
    visitor.visitGroup(*this);
    visitor << log::unindent() << "}\n";
}

} // namespace physics
} // namespace slon
