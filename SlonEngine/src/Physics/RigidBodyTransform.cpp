#include "stdafx.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/RigidBodyTransform.h"

namespace slon {
namespace physics {

RigidBodyTransform::RigidBodyTransform(physics::RigidBody* rigidBody_) :
    rigidBody(rigidBody_),
    absolute(false)
{
}

bool RigidBodyTransform::isAbsolute() const
{
    return absolute;
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
            visitor << "unnamed(" << rigidBody.get() << ")";
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
