#include "stdafx.h"
#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Database/Bullet/Bullet.h"
#include "Filesystem/File.h"
#include "Physics/PhysicsManager.h"
#include "Physics/PhysicsModel.h"
#include "Physics/Bullet/BulletCommon.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"
#include "Utility/error.hpp"
#include <bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>
#include <bullet/LinearMath/btSerializer.h>

DECLARE_AUTO_LOGGER("Database.Bullet")

namespace slon {
namespace database {
namespace detail {

physics::physics_model_ptr BulletLoader::load(filesystem::File* file)
{
    physics::BulletDynamicsWorld& world   = *physics::currentPhysicsManager().getDynamicsWorld()->getImpl();
    btDynamicsWorld*              btWorld = &world.getBtDynamicsWorld();
    	
    // read file content
    file->open(filesystem::File::in | filesystem::File::binary);
	std::string fileContent(file->size(), ' ');
    file->read( &fileContent[0], fileContent.size() );
    file->close();

    std::auto_ptr<btBulletWorldImporter> fileLoader( new btBulletWorldImporter( &world.getBtDynamicsWorld() ) );
    if ( !fileLoader->loadFileFromMemory( (char*)fileContent.data(), fileContent.length() ) ) {
        throw file_error(AUTO_LOGGER, "Can't load bullet physics file");
    }

    // enumerate objects and add them to the scene model
    physics::physics_model_ptr sceneModel(new physics::PhysicsModel);
    for (int i = 0; i<fileLoader->getNumRigidBodies(); ++i) 
    {
        btCollisionObject* collisionObject = fileLoader->getRigidBodyByIndex(i);
        if ( btRigidBody* rigidBody = dynamic_cast<btRigidBody*>(collisionObject) ) 
        {
			physics::RigidBody::state_desc desc;
			desc.mass            = rigidBody->getInvMass() > 0 ? 1 / rigidBody->getInvMass() : 0;
			desc.inertia         = math::Vector3r( 1 / rigidBody->getInvInertiaDiagLocal().x(), 
										           1 / rigidBody->getInvInertiaDiagLocal().y(), 
										           1 / rigidBody->getInvInertiaDiagLocal().z() );
			btTransform transform;
			if ( rigidBody->getMotionState() ) {
				rigidBody->getMotionState()->getWorldTransform(transform);
			}
			else {
				transform = rigidBody->getWorldTransform();
			}
			desc.transform       = to_mat( transform );
			desc.angularVelocity = to_vec( rigidBody->getAngularVelocity() );
			desc.linearVelocity  = to_vec( rigidBody->getLinearVelocity() );
			if (rigidBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) {
				desc.type = physics::RigidBody::DT_KINEMATIC;
			}
			else if (rigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) {
				desc.type = physics::RigidBody::DT_STATIC;
			}
			else {
				desc.type = physics::RigidBody::DT_DYNAMIC;
			}
			desc.collisionShape  = physics::createCollisionShape( *rigidBody->getCollisionShape() );

			sceneModel->addCollisionObject( new physics::RigidBody(desc), fileLoader->getNameForPointer(rigidBody) );
        }
    }

    // enumerate constraints and add them to the scene
    for (int i = 0; i<fileLoader->getNumConstraints(); ++i) 
    {
        boost::scoped_ptr<btTypedConstraint> constraint( fileLoader->getConstraintByIndex(i) );
        switch ( constraint->getObjectType() )
        {
            case HINGE_CONSTRAINT_TYPE:
            {
                btHingeConstraint* hConstraint = static_cast<btHingeConstraint*>(constraint.get());

                physics::Constraint::state_desc desc;
                desc.rigidBodies[0]   = reinterpret_cast<physics::BulletRigidBody*>(hConstraint->getRigidBodyA().getUserPointer())->getInterface();
                desc.rigidBodies[1]   = reinterpret_cast<physics::BulletRigidBody*>(hConstraint->getRigidBodyB().getUserPointer())->getInterface();
                desc.frames[0]        = physics::to_mat( hConstraint->getAFrame() );
                desc.frames[1]        = physics::to_mat( hConstraint->getBFrame() );
                desc.linearLimits[0]  = math::Vector3r(0);
                desc.linearLimits[1]  = math::Vector3r(0);
                desc.angularLimits[0] = math::Vector3r( 0, 0, hConstraint->getLowerLimit() );
                desc.angularLimits[1] = math::Vector3r( 0, 0, hConstraint->getUpperLimit() );
                desc.name             = fileLoader->getNameForPointer(hConstraint);

                btWorld->removeConstraint( hConstraint );
                sceneModel->addConstraint( new physics::Constraint(desc) );
                break;
            }

            case CONETWIST_CONSTRAINT_TYPE:
            {
                btConeTwistConstraint* ctConstraint = static_cast<btConeTwistConstraint*>(constraint.get());

                physics::Constraint::state_desc desc;
                desc.rigidBodies[0]   = reinterpret_cast<physics::BulletRigidBody*>(ctConstraint->getRigidBodyA().getUserPointer())->getInterface();
                desc.rigidBodies[1]   = reinterpret_cast<physics::BulletRigidBody*>(ctConstraint->getRigidBodyB().getUserPointer())->getInterface();
                desc.frames[0]        = physics::to_mat( ctConstraint->getAFrame() );
                desc.frames[1]        = physics::to_mat( ctConstraint->getBFrame() );
                desc.linearLimits[0]  = math::Vector3r(0);
                desc.linearLimits[1]  = math::Vector3r(0);
                desc.angularLimits[0] = math::Vector3r( -ctConstraint->getTwistSpan(), -ctConstraint->getSwingSpan1(), -ctConstraint->getSwingSpan2() );
                desc.angularLimits[1] = -desc.angularLimits[0];
                desc.name             = fileLoader->getNameForPointer(ctConstraint);

                btWorld->removeConstraint( ctConstraint );
                sceneModel->addConstraint( new physics::Constraint(desc) );
                break;
            }

            case D6_CONSTRAINT_TYPE:
            {
                btGeneric6DofConstraint* gConstraint = static_cast<btGeneric6DofConstraint*>(constraint.get());

                physics::Constraint::state_desc desc;
                desc.rigidBodies[0]   = reinterpret_cast<physics::BulletRigidBody*>(gConstraint->getRigidBodyA().getUserPointer())->getInterface();
                desc.rigidBodies[1]   = reinterpret_cast<physics::BulletRigidBody*>(gConstraint->getRigidBodyB().getUserPointer())->getInterface();
                desc.frames[0]        = physics::to_mat( gConstraint->getFrameOffsetA() );
                desc.frames[1]        = physics::to_mat( gConstraint->getFrameOffsetB() );
                desc.linearLimits[0]  = to_vec(gConstraint->getTranslationalLimitMotor()->m_lowerLimit);
                desc.linearLimits[1]  = to_vec(gConstraint->getTranslationalLimitMotor()->m_upperLimit);
                desc.angularLimits[0] = math::Vector3r(gConstraint->getRotationalLimitMotor(0)->m_loLimit, 
													   gConstraint->getRotationalLimitMotor(1)->m_loLimit, 
													   gConstraint->getRotationalLimitMotor(2)->m_loLimit);
                desc.angularLimits[1] = math::Vector3r(gConstraint->getRotationalLimitMotor(0)->m_hiLimit, 
													   gConstraint->getRotationalLimitMotor(1)->m_hiLimit, 
													   gConstraint->getRotationalLimitMotor(2)->m_hiLimit);
                desc.name             = fileLoader->getNameForPointer(gConstraint);

                btWorld->removeConstraint( gConstraint );
                sceneModel->addConstraint( new physics::Constraint(desc) );
                break;
            }

            default:
                AUTO_LOGGER_MESSAGE(log::S_ERROR, "Unsupported constraint type. Skipping '" << fileLoader->getNameForPointer(constraint.get()) << "'");
                btWorld->removeConstraint( constraint.get() );
                break;
        };
    }

    return sceneModel;
}

void BulletSaver::save(physics::physics_model_ptr model, filesystem::File* file)
{
    using namespace physics;

	btDefaultSerializer* btSerializer = new btDefaultSerializer(1024*1024*5);
	/*
    btSerializer->startSerialization();
    {
        // serialize collision shapes
        std::set<btCollisionShape*>	serializedShapes;

        for (PhysicsModel::collision_object_iterator iter  = model->firstCollisionObject();
                                                     iter != model->endCollisionObject();
                                                     ++iter)
        {
            BulletRigidBody&  rigidBody = *static_cast<physics::RigidBody*>(iter->first.get())->getImpl();
            btRigidBody&      btRB      = rigidBody.getBtRigidBody();
            btCollisionShape* btShape   = btRB.getCollisionShape();

            if (serializedShapes.count(btShape) == 0)
            {
	            serializedShapes.insert(btShape);
                btSerializer->registerNameForPointer(btShape, iter->second.c_str());
	            btShape->serializeSingleShape(btSerializer);
            }
        }

        // serialize rigid bodies
        for (PhysicsModel::collision_object_iterator iter  = model->firstCollisionObject();
                                                     iter != model->endCollisionObject();
                                                     ++iter)
        {
            btRigidBody&  btRB       = static_cast<physics::RigidBody*>(iter->first.get())->getImpl()->getBtRigidBody();
			btChunk*      chunk      = btSerializer->allocate(btRB.calculateSerializeBufferSize(), 1);
			const char*   structType = btRB.serialize(chunk->m_oldPtr, btSerializer);
			btSerializer->finalizeChunk(chunk, structType, BT_RIGIDBODY_CODE, &btRB);
        }

        // serialize constraints
        for (PhysicsModel::constraint_iterator iter  = model->firstConstraint();
                                               iter != model->endConstraint();
                                               ++iter)
        {
            btGeneric6DofConstraint& btConstraint = static_cast<BulletConstraint&>(*(*iter)->getImpl()).getBtConstraint();
	        btChunk*                 chunk        = btSerializer->allocate(btConstraint.calculateSerializeBufferSize(), 1);
	        const char*              structType   = btConstraint.serialize(chunk->m_oldPtr, btSerializer);
	        btSerializer->finalizeChunk(chunk, structType, BT_CONSTRAINT_CODE, &btConstraint);
        }
    }
    btSerializer->finishSerialization();
	*/
    if ( file->open(filesystem::File::out | filesystem::File::binary) )
    {
        file->write((const char*)btSerializer->getBufferPointer(), btSerializer->getCurrentBufferSize());
        file->close();
    }
}

} // namespace detail
} // namespace database
} // namespace slon
