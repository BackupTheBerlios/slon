#include "stdafx.h"
#include "Database/Archive.h"
#include "Log/Formatters.h"
#include "Log/LogVisitor.h"
#include "Physics/Constraint.h"
#include "Physics/ConstraintNode.h"
#include "Physics/DynamicsWorld.h"
#include "Realm/Location.h"

namespace slon {
namespace physics {

ConstraintNode::ConstraintNode(const constraint_ptr& constraint_) :
    constraint(constraint_)
{
}

const char* ConstraintNode::serialize(database::OArchive& ar) const
{
	// serialize base class
	Node::serialize(ar);

	// serialize data
    ar.writeSerializable(constraint.get());
    return "ConstraintNode";
}

void ConstraintNode::deserialize(database::IArchive& ar)
{
	// deserialize base class
	Node::deserialize(ar);

	// deserialize data
    constraint = ar.readSerializable<physics::Constraint>();
}

void ConstraintNode::accept(log::LogVisitor& visitor) const
{
    visitor << "ConstraintNode";
    if ( getName() != "" ) {
        visitor << " '" << getName() << "'";
    }
    visitor << "\n{\n" << log::indent()
            << "constraint =";
    if (constraint)
    {
        if ( constraint->getName() != "" ) {
            visitor << "'" << constraint->getName() << "'";
        }
        else {
            visitor << "unnamed(" << constraint << ")";
        }
    }
    else {
        visitor << "0";
    }
    visitor << "\n" << log::unindent() << "}\n";
}

void ConstraintNode::accept(realm::EventVisitor& ev)
{
    if ( !ev.getPhysicsToggle() || !constraint ) {
        return;
    }

	if ( realm::Location* location = ev.getLocation() )
	{
		if ( physics::DynamicsWorld* world = location->getDynamicsWorld() )
		{
			if (ev.getType() == realm::EventVisitor::WORLD_ADD) {
				world->addConstraint( static_cast<Constraint*>(constraint.get()) );
			}
			else if (ev.getType() == realm::EventVisitor::WORLD_REMOVE) {
				world->removeConstraint( static_cast<Constraint*>(constraint.get()) );
			}
		}
	}
}

} // namespace physics
} // namespace slon
