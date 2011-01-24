#include "stdafx.h"
#define NOMINMAX
#include "Engine.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"
#include "Physics/Bullet/BulletRotationalServoMotor.h"
#include "Physics/Bullet/BulletRotationalSpringMotor.h"
#include "Physics/Bullet/BulletRotationalVelocityMotor.h"
#include <sgl/Math/Utility.hpp>

namespace {

    using namespace slon;
    using namespace physics;

    btGeneric6DofConstraint* createBulletConstraint(const Constraint::state_desc& desc)
    {
        assert(desc.rigidBodies[0] && desc.rigidBodies[1] && "Constraint must specify affected rigid bodies");

        btRigidBody& rigidBodyA = *static_cast<BulletRigidBody*>(desc.rigidBodies[0])->getBtRigidBody();
        btRigidBody& rigidBodyB = *static_cast<BulletRigidBody*>(desc.rigidBodies[1])->getBtRigidBody();

        btGeneric6DofConstraint* constraint = new btGeneric6DofConstraint( rigidBodyA,
                                                                           rigidBodyB,
                                                                           to_bt_mat(desc.frames[0]),
                                                                           to_bt_mat(desc.frames[1]),
                                                                           false );

        for (int i = 0; i<3; ++i)
        {
            if ( desc.linearLimits[0][i] == -std::numeric_limits<float>::infinity() && desc.linearLimits[1][i] == std::numeric_limits<float>::infinity() ) {
                constraint->setLimit(i, 1.0f, -1.0f); // unlocked
            }
            else {
                constraint->setLimit(i, desc.linearLimits[0][i], desc.linearLimits[1][i]);
            }       
            
            if ( desc.angularLimits[0][i] == -std::numeric_limits<float>::infinity() && desc.angularLimits[1][i] == std::numeric_limits<float>::infinity() ) {
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

BulletConstraint::BulletConstraint(const bullet_constraint_ptr& constraint_,
                                   const std::string&           name_) :
    constraint(constraint_)
{
    assert(constraint);

    desc.name           = name_;
    desc.rigidBodies[0] = reinterpret_cast<RigidBody*>( constraint->getRigidBodyA().getUserPointer() );
    desc.rigidBodies[1] = reinterpret_cast<RigidBody*>( constraint->getRigidBodyB().getUserPointer() );

    if ( btGeneric6DofConstraint* genericConstraint = dynamic_cast<btGeneric6DofConstraint*>( constraint.get() ) )
    {
        desc.frames[0]       = to_mat( genericConstraint->getFrameOffsetA() );
        desc.frames[1]       = to_mat( genericConstraint->getFrameOffsetB() );

        desc.linearLimits[0] = to_vec( genericConstraint->getTranslationalLimitMotor()->m_lowerLimit );
        desc.linearLimits[1] = to_vec( genericConstraint->getTranslationalLimitMotor()->m_upperLimit );

        desc.angularLimits[0] = math::Vector3f( genericConstraint->getRotationalLimitMotor(0)->m_loLimit,
                                                genericConstraint->getRotationalLimitMotor(1)->m_loLimit,
                                                genericConstraint->getRotationalLimitMotor(2)->m_loLimit );

        desc.angularLimits[1] = math::Vector3f( genericConstraint->getRotationalLimitMotor(0)->m_hiLimit,
                                                genericConstraint->getRotationalLimitMotor(1)->m_hiLimit,
                                                genericConstraint->getRotationalLimitMotor(2)->m_hiLimit );
    }

    // link with rigid body
    for (int i = 0; i<2; ++i)
    {
        if ( BulletRigidBody* rb = static_cast<BulletRigidBody*>(desc.rigidBodies[i]) )
        {
            BulletRigidBody::constraint_iterator iter = std::find(rb->constraints.begin(), rb->constraints.end(), this);
            if ( iter == rb->constraints.end() ) {
                rb->constraints.push_back(this);
            }
        }
    }
}

BulletConstraint::BulletConstraint(const state_desc& _desc)
{
    reset(_desc);
}

BulletConstraint::~BulletConstraint()
{
    // remove constraint references
    if (desc.rigidBodies[0]) unlink(*desc.rigidBodies[0]);
    if (desc.rigidBodies[1]) unlink(*desc.rigidBodies[1]);
}

void BulletConstraint::unlink(const RigidBody& rigidBody)
{
    if (desc.rigidBodies[0] == &rigidBody) 
    {
        static_cast<BulletRigidBody*>(desc.rigidBodies[0])->removeConstraint(*this);
        desc.rigidBodies[0] = 0;
    }
    else if (desc.rigidBodies[1] == &rigidBody) 
    {
        static_cast<BulletRigidBody*>(desc.rigidBodies[1])->removeConstraint(*this);
        desc.rigidBodies[1] = 0;
    }
    else {
        return;
    }

    // destroy invalid constraint
    destroy();
}

void BulletConstraint::destroy()
{
    if (constraint) 
    {
        assert(dynamicsWorld);

        // destroy
        dynamicsWorld->getBtDynamicsWorld().removeConstraint( constraint.get() );
        constraint.reset();
    }
}

void BulletConstraint::reset(const state_desc& _desc)
{
    destroy();

    // get bodies
    assert( _desc.rigidBodies[0] && _desc.rigidBodies[1] && "Constraint must specify affected rigid bodies");  
    BulletRigidBody* rigidBodyA = static_cast<BulletRigidBody*>(_desc.rigidBodies[0]);
    BulletRigidBody* rigidBodyB = static_cast<BulletRigidBody*>(_desc.rigidBodies[1]);

    // setup constraint
    bullet_constraint_ptr newConstraint( createBulletConstraint(_desc) );
    assert( rigidBodyA->dynamicsWorld == rigidBodyB->dynamicsWorld && "Linked bodies must be in same dynamics world" );
    {
        dynamicsWorld = rigidBodyA->dynamicsWorld;
        dynamicsWorld->getBtDynamicsWorld().addConstraint(newConstraint.get());
        
        // attach constraints to rigid body
        for (int i = 0; i<2; ++i) 
        {
            BulletRigidBody* oldRigidBody = static_cast<BulletRigidBody*>( desc.rigidBodies[i] );
            BulletRigidBody* rigidBody    = static_cast<BulletRigidBody*>( _desc.rigidBodies[i] );
            if (oldRigidBody != rigidBody) 
            {
                rigidBody->constraints.push_back(this);
                if (oldRigidBody) {
                    oldRigidBody->removeConstraint(*this);
                }
            }
        }
    }
    constraint.swap(newConstraint);

    // create motors
    for (int i = 0; i<3; ++i)
    {
        if ( fabs(desc.angularLimits[0][i] - desc.angularLimits[1][i]) > 0.02f && rotationalMotors[i] ) {
            rotationalMotors[i]->reset(this, i);
        }
        else {
            rotationalMotors[i].reset();
        }
    }

    if (newConstraint) 
    {
        dynamicsWorld->getBtDynamicsWorld().removeConstraint( newConstraint.get() );
        newConstraint.reset();
    }

    desc = _desc;
}

const Motor* BulletConstraint::getMotor(Motor::TYPE motor) const
{
    assert(motor < 6); 
    
    if (motor < 3) {
        return 0;
    }
    
    return rotationalMotors[motor - 3] ? rotationalMotors[motor - 3]->asMotor() : 0; 
}

Motor* BulletConstraint::getMotor(Motor::TYPE motor)            
{ 
    assert(motor < 6); 
    
    if (motor < 3) {
        return 0;
    }
    
    return rotationalMotors[motor - 3] ? rotationalMotors[motor - 3]->asMotor() : 0; 
}

ServoMotor* BulletConstraint::createServoMotor(Motor::TYPE motor)
{
    btGeneric6DofConstraint* genericConstraint = (btGeneric6DofConstraint*)constraint.get();
    if (motor >= 3) 
    {
        // rotational
        int i = motor - 3;
        if ( fabs(desc.angularLimits[0][i] - desc.angularLimits[1][i]) > 0.02f ) 
        {
            BulletRotationalServoMotor* m = new BulletRotationalServoMotor(this, i);
            rotationalMotors[i].reset(m);
            return m;
        }
    }
    else 
    {
        // translational
    }

    return 0;
}

VelocityMotor* BulletConstraint::createVelocityMotor(Motor::TYPE motor)
{
    btGeneric6DofConstraint* genericConstraint = (btGeneric6DofConstraint*)constraint.get();
    if (motor >= 3) 
    {
        // rotational
        int i = motor - 3;
        if ( fabs(desc.angularLimits[0][i] - desc.angularLimits[1][i]) > 0.02f ) 
        {
            BulletRotationalVelocityMotor* m = new BulletRotationalVelocityMotor(this, i);
            rotationalMotors[i].reset(m);
            return m;
        }
    }
    else 
    {
        // translational
    }

    return 0;
}

SpringMotor* BulletConstraint::createSpringMotor(Motor::TYPE motor)
{
    btGeneric6DofConstraint* genericConstraint = (btGeneric6DofConstraint*)constraint.get();
    if (motor >= 3) 
    {
        // rotational
        int i = motor - 3;
        if ( fabs(desc.angularLimits[0][i] - desc.angularLimits[1][i]) > 0.02f ) 
        {
            BulletRotationalSpringMotor* m = new BulletRotationalSpringMotor(this, i);
            rotationalMotors[i].reset(m);
            return m;
        }
    }
    else 
    {
        // translational
    }

    return 0;
}

math::Vector3f BulletConstraint::getAxis(unsigned int axis) const
{
    assert(axis < 3);
    return to_vec( constraint->getAxis(axis) );
}

float BulletConstraint::getRotationAngle(unsigned int axis) const
{
    return constraint->getRotationalLimitMotor(axis)->m_currentPosition;
}

BulletConstraint::AXIS_RESTRICTION BulletConstraint::getAxisRestriction(unsigned int axis) const
{
    assert(axis < 3);
    if (!rotationalMotors[axis]) {
        return AXIS_LOCKED;
    }

    const btRotationalLimitMotor* motor = constraint->getRotationalLimitMotor(axis);
    if (motor->m_loLimit > motor->m_hiLimit) { 
        return AXIS_FREE;
    }

    return AXIS_RESTRICTED;
}

void BulletConstraint::accept(BulletSolverCollector& collector)
{
    for (int i = 0; i<3; ++i)
    {
        if (rotationalMotors[i]) {
            rotationalMotors[i]->accept(collector);
        }
    }
}

} // namespace physics
} // namespace slon
