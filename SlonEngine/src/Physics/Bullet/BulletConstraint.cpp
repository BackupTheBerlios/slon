#include "stdafx.h"
#define NOMINMAX
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include <sgl/Math/Utility.hpp>

namespace {

    using namespace slon;
    using namespace physics;

    btGeneric6DofConstraint* createBulletConstraint(const Constraint::DESC& desc)
    {
        assert(desc.rigidBodies[0] && desc.rigidBodies[1] && "Constraint must specify affected rigid bodies");

        btRigidBody& rigidBodyA = desc.rigidBodies[0]->getImpl()->getBtRigidBody();
        btRigidBody& rigidBodyB = desc.rigidBodies[1]->getImpl()->getBtRigidBody();

        btGeneric6DofConstraint* constraint = new btGeneric6DofConstraint( rigidBodyA,
                                                                           rigidBodyB,
                                                                           to_bt_mat(desc.frames[0]),
                                                                           to_bt_mat(desc.frames[1]),
                                                                           false );

        for (int i = 0; i<3; ++i)
        {
            if ( desc.linearLimits[0][i] == -std::numeric_limits<real>::infinity() && desc.linearLimits[1][i] == std::numeric_limits<real>::infinity() ) {
                constraint->setLimit(i, 1.0f, -1.0f); // unlocked
            }
            else {
                constraint->setLimit(i, desc.linearLimits[0][i], desc.linearLimits[1][i]);
            }       
            
            if ( desc.angularLimits[0][i] == -std::numeric_limits<real>::infinity() && desc.angularLimits[1][i] == std::numeric_limits<real>::infinity() ) {
                constraint->setLimit(i + 3, 1.0f, -1.0f); // unlocked
            }
            else {
                constraint->setLimit(i + 3, desc.angularLimits[0][i], desc.angularLimits[1][i]);
            }       
        }

        return constraint;
    }

} // anonymous namespace

namespace slon {
namespace physics {

BulletConstraint::BulletConstraint(Constraint*              pInterface_,
					               BulletDynamicsWorld*     dynamicsWorld_,
					               btGeneric6DofConstraint* constraint_,
                                   const std::string&       name_) 
:	pInterface(pInterface_)
,	dynamicsWorld(dynamicsWorld_)
,	constraint(constraint_)
{
    assert(constraint);
	Constraint::DESC& desc = pInterface->desc;

    desc.name           = name_;
    desc.rigidBodies[0] = reinterpret_cast<RigidBody*>( constraint->getRigidBodyA().getUserPointer() );
    desc.rigidBodies[1] = reinterpret_cast<RigidBody*>( constraint->getRigidBodyB().getUserPointer() );

    if ( btGeneric6DofConstraint* genericConstraint = dynamic_cast<btGeneric6DofConstraint*>( constraint.get() ) )
    {
        desc.frames[0]       = to_mat( genericConstraint->getFrameOffsetA() );
        desc.frames[1]       = to_mat( genericConstraint->getFrameOffsetB() );

        desc.linearLimits[0] = to_vec( genericConstraint->getTranslationalLimitMotor()->m_lowerLimit );
        desc.linearLimits[1] = to_vec( genericConstraint->getTranslationalLimitMotor()->m_upperLimit );

        desc.angularLimits[0] = math::Vector3r( genericConstraint->getRotationalLimitMotor(0)->m_loLimit,
                                                genericConstraint->getRotationalLimitMotor(1)->m_loLimit,
                                                genericConstraint->getRotationalLimitMotor(2)->m_loLimit );

        desc.angularLimits[1] = math::Vector3r( genericConstraint->getRotationalLimitMotor(0)->m_hiLimit,
                                                genericConstraint->getRotationalLimitMotor(1)->m_hiLimit,
                                                genericConstraint->getRotationalLimitMotor(2)->m_hiLimit );
    }
}

BulletConstraint::BulletConstraint(Constraint*          pInterface_,
					               BulletDynamicsWorld* dynamicsWorld_)
:	pInterface(pInterface_)
,	dynamicsWorld(dynamicsWorld_)
{
	assert(dynamicsWorld);
    const Constraint::DESC& desc = pInterface->desc;

    // get bodies
    assert( desc.rigidBodies[0] && desc.rigidBodies[1] && "Constraint must specify affected rigid bodies.");  
    BulletRigidBody* rigidBodyA = static_cast<BulletRigidBody*>( desc.rigidBodies[0]->getImpl() );
    BulletRigidBody* rigidBodyB = static_cast<BulletRigidBody*>( desc.rigidBodies[1]->getImpl() );
    assert( rigidBodyA && rigidBodyB && "Rigid bodies should be in world when instantiating constraint." );

    // setup constraint
    constraint.reset( createBulletConstraint(desc) );
    dynamicsWorld->getBtDynamicsWorld().addConstraint( constraint.get() );
}

BulletConstraint::~BulletConstraint()
{
	dynamicsWorld->getBtDynamicsWorld().removeConstraint( constraint.get() );
}

math::Vector3r BulletConstraint::getAxis(unsigned int axis) const
{
    assert(axis < 3);
    return to_vec( constraint->getAxis(axis) );
}

real BulletConstraint::getPosition(Constraint::DOF dof) const
{
    if (dof < 3) {
		return 0.0f;/*constraint->getTranslationalLimitMotor(dof)->m*/
	}
	else {
		return constraint->getRotationalLimitMotor(dof - 3)->m_currentPosition;
	}
}

} // namespace physics
} // namespace slon
