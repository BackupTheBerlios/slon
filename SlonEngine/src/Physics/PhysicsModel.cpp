#include "stdafx.h"
#include "Physics/PhysicsModel.h"
#include <boost/iterator/indirect_iterator.hpp>

namespace slon {
namespace physics {

PhysicsModel::PhysicsModel()
{
}

PhysicsModel::~PhysicsModel()
{
}

bool PhysicsModel::addRigidBody(RigidBody* rigidBody)
{
    if (rigidBodies.insert(rigidBody).second) 
    {
        // add constraints
        for (RigidBody::constraint_iterator iter  = rigidBody->firstConstraint();
                                            iter != rigidBody->endConstraint();
                                            ++iter)
        {
            constraints.insert(*iter);
        }

        return true;
    }

    return false;
}

bool PhysicsModel::removeRigidBody(RigidBody* rigidBody)
{
    // can't be in the set if no one use it
    if (rigidBody->use_count() == 0) {
        return false;
    }

    // lock copy
    rigid_body_ptr rb(rigidBody);
    if (rigidBodies.erase(rb) == 1) 
    {
        // enumerate constraints
        constraints.clear();
        for (rigid_body_iterator iter = rigidBodies.begin(); iter != rigidBodies.end(); ++iter) 
        {
            for (RigidBody::constraint_iterator constIter  = (*iter)->firstConstraint();
                                                constIter != (*iter)->endConstraint();
                                                ++constIter)
            {
                constraints.insert(*constIter);
            }
        }

        return true;
    }

    return false;
}

bool PhysicsModel::addConstraint(Constraint* constraint)
{
    return constraints.insert(constraint).second;
}

bool PhysicsModel::removeConstraint(Constraint* constraint)
{
    return (constraints.erase(constraint) == 1);
}

PhysicsModel::rigid_body_iterator PhysicsModel::findRigidBody(RigidBody* rigidBody)
{
    return rigidBodies.find(rigidBody);
}

PhysicsModel::constraint_iterator PhysicsModel::findConstraint(Constraint* constraint)
{
    return constraints.find(constraint);
}

PhysicsModel::rigid_body_iterator PhysicsModel::findRigidBody(const std::string& name)
{
    return std::find_if( boost::make_indirect_iterator( rigidBodies.begin() ),
                         boost::make_indirect_iterator( rigidBodies.end() ),
                         boost::bind(&RigidBody::getName, _1) == name ).base();
}

PhysicsModel::constraint_iterator PhysicsModel::findConstraint(const std::string& name)
{
    return std::find_if( boost::make_indirect_iterator( constraints.begin() ),
                         boost::make_indirect_iterator( constraints.end() ),
                         boost::bind(&Constraint::getName, _1) == name ).base();
}

void PhysicsModel::toggleSimulation(bool toggle)
{
    std::for_each( boost::make_indirect_iterator( rigidBodies.begin() ),
                   boost::make_indirect_iterator( rigidBodies.end() ),
                   boost::bind(&RigidBody::toggleSimulation, _1, toggle) );
}

} // namespace physics
} // namespace slon
