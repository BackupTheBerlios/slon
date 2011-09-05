#ifndef __SLON_ENGINE_PHYSICS_CONSTRAINT_NODE_H__
#define __SLON_ENGINE_PHYSICS_CONSTRAINT_NODE_H__

#include "../Realm/EventVisitor.h"
#include "../Scene/AcceptVisitor.hpp"
#include "../Scene/Node.h"
#include "Forward.h"

namespace slon {
namespace physics {

class SLON_PUBLIC ConstraintNode :
    public scene::Node,
    public scene::AcceptVisitor<realm::EventVisitor>
{
public:
    ConstraintNode(const constraint_ptr& constraint = constraint_ptr());

    // Override Serializable
    const char* serialize(database::OArchive& ar) const;
    void        deserialize(database::IArchive& ar);

    // Override Node
    void accept(log::LogVisitor& visitor) const;
    void accept(realm::EventVisitor& visitor);

    /** Set constraint. */
    void setConstraint(const constraint_ptr& constraint_) { constraint = constraint_; }

    /** Get constraint. */
    Constraint* getConstraint() { return constraint.get(); }

    /** Get constraint. */
    const Constraint* getConstraint() const { return constraint.get(); }

private:
    constraint_ptr constraint;
};

} // namespace physics
} // namespace slon

#endif // __SLON_ENGINE_PHYSICS_CONSTRAINT_NODE_H__
