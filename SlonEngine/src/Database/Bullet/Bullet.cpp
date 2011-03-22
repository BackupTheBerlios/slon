#include "stdafx.h"
#include "Database/Bullet/Bullet.h"
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

__DEFINE_LOGGER__("Database.Bullet")

namespace slon {
namespace database {
namespace detail {

physics::physics_model_ptr BulletLoader::load(std::istream& stream)
{
    physics::BulletDynamicsWorld& world   = static_cast<physics::BulletDynamicsWorld&>( *physics::currentPhysicsManager().getDynamicsWorld() );
    btDynamicsWorld*              btWorld = &world.getBtDynamicsWorld();
    	
    // read file content
	std::ostringstream buffer;
	buffer << stream.rdbuf();

    std::auto_ptr<btBulletWorldImporter> fileLoader( new btBulletWorldImporter( &world.getBtDynamicsWorld() ) );
    if ( !fileLoader->loadFileFromMemory( (char*)buffer.str().data(), buffer.str().length() ) ) {
        throw file_not_found_error(logger, "Can't load bullet physics file");
    }

    // enumerate objects and add them to the scene model
    physics::physics_model_ptr sceneModel(new physics::PhysicsModel);
    for (int i = 0; i<fileLoader->getNumRigidBodies(); ++i) 
    {
        btCollisionObject* collisionObject = fileLoader->getRigidBodyByIndex(i);
        if ( btRigidBody* rigidBody = dynamic_cast<btRigidBody*>(collisionObject) ) 
        {
            sceneModel->addRigidBody( new physics::BulletRigidBody( physics::BulletRigidBody::rigid_body_ptr(rigidBody), 
                                                                    fileLoader->getNameForPointer(rigidBody),
                                                                    &world ) );
        }
    }

    // enumerate constraints and add them to the scene
    for (int i = 0; i<fileLoader->getNumConstraints(); ++i) 
    {
        boost::shared_ptr<btTypedConstraint> constraint( fileLoader->getConstraintByIndex(i) );
        switch ( constraint->getObjectType() )
        {
            case HINGE_CONSTRAINT_TYPE:
            {
                btHingeConstraint* hConstraint = static_cast<btHingeConstraint*>(constraint.get());

                physics::Constraint::state_desc desc;
                desc.rigidBodies[0]   = (physics::BulletRigidBody*)hConstraint->getRigidBodyA().getUserPointer();
                desc.rigidBodies[1]   = (physics::BulletRigidBody*)hConstraint->getRigidBodyB().getUserPointer();
                desc.frames[0]        = physics::to_mat( hConstraint->getAFrame() );
                desc.frames[1]        = physics::to_mat( hConstraint->getBFrame() );
                desc.linearLimits[0]  = math::Vector3r(0);
                desc.linearLimits[1]  = math::Vector3r(0);
                desc.angularLimits[0] = math::Vector3r( 0, 0, hConstraint->getLowerLimit() );
                desc.angularLimits[1] = math::Vector3r( 0, 0, hConstraint->getUpperLimit() );
                desc.name             = fileLoader->getNameForPointer(hConstraint);

                btWorld->removeConstraint( hConstraint );
                sceneModel->addConstraint( world.createConstraint(desc) );
                break;
            }

            case CONETWIST_CONSTRAINT_TYPE:
            {
                btConeTwistConstraint* ctConstraint = static_cast<btConeTwistConstraint*>(constraint.get());

                physics::Constraint::state_desc desc;
                desc.rigidBodies[0]   = (physics::BulletRigidBody*)ctConstraint->getRigidBodyA().getUserPointer();
                desc.rigidBodies[1]   = (physics::BulletRigidBody*)ctConstraint->getRigidBodyB().getUserPointer();
                desc.frames[0]        = physics::to_mat( ctConstraint->getAFrame() );
                desc.frames[1]        = physics::to_mat( ctConstraint->getBFrame() );
                desc.linearLimits[0]  = math::Vector3r(0);
                desc.linearLimits[1]  = math::Vector3r(0);
                desc.angularLimits[0] = math::Vector3r( -ctConstraint->getTwistSpan(), -ctConstraint->getSwingSpan1(), -ctConstraint->getSwingSpan2() );
                desc.angularLimits[1] = -desc.angularLimits[0];
                desc.name             = fileLoader->getNameForPointer(ctConstraint);

                btWorld->removeConstraint( ctConstraint );
                sceneModel->addConstraint( world.createConstraint(desc) );
                break;
            }

            case D6_CONSTRAINT_TYPE:
            {
                sceneModel->addConstraint( new physics::BulletConstraint( boost::shared_static_cast<btGeneric6DofConstraint>(constraint), fileLoader->getNameForPointer(constraint.get()) ) );
                break;
            }

            default:
                logger << log::S_ERROR << "Unsupported constraint type. Skipping '" << fileLoader->getNameForPointer(constraint.get()) << "'";
                btWorld->removeConstraint( constraint.get() );
                break;
        };
    }

    return sceneModel;
}

void BulletSaver::save(physics::physics_model_ptr model, std::ostream& sink)
{
    using namespace physics;

	btDefaultSerializer* btSerializer = new btDefaultSerializer(1024*1024*5);

    btSerializer->startSerialization();
    {
        // serialize collision shapes
        std::set<btCollisionShape*>	serializedShapes;

        for (PhysicsModel::rigid_body_iterator iter  = model->firstRigidBody();
                                               iter != model->endRigidBody();
                                               ++iter)
        {
            BulletRigidBody&  rigidBody = static_cast<BulletRigidBody&>(**iter);
            btRigidBody*      btRB      = rigidBody.getBtRigidBody().get();
            btCollisionShape* btShape   = btRB->getCollisionShape();

            if (serializedShapes.count(btShape) == 0)
            {
	            serializedShapes.insert(btShape);
                btSerializer->registerNameForPointer(btShape, rigidBody.getTarget().c_str());
	            btShape->serializeSingleShape(btSerializer);
            }
        }

        // serialize rigid bodies
        for (PhysicsModel::rigid_body_iterator iter  = model->firstRigidBody();
                                               iter != model->endRigidBody();
                                               ++iter)
        {
            btRigidBody*  btRB       = static_cast<BulletRigidBody&>(**iter).getBtRigidBody().get();
			btChunk*      chunk      = btSerializer->allocate(btRB->calculateSerializeBufferSize(), 1);
			const char*   structType = btRB->serialize(chunk->m_oldPtr, btSerializer);
			btSerializer->finalizeChunk(chunk, structType, BT_RIGIDBODY_CODE, btRB);
        }

        // serialize constraints
        for (PhysicsModel::constraint_iterator iter  = model->firstConstraint();
                                               iter != model->endConstraint();
                                               ++iter)
        {
            btGeneric6DofConstraint* btConstraint = static_cast<BulletConstraint&>(**iter).getBtConstraint();
	        btChunk*                 chunk        = btSerializer->allocate(btConstraint->calculateSerializeBufferSize(), 1);
	        const char*              structType   = btConstraint->serialize(chunk->m_oldPtr, btSerializer);
	        btSerializer->finalizeChunk(chunk, structType, BT_CONSTRAINT_CODE, btConstraint);
        }
    }
    btSerializer->finishSerialization();

    sink.write((const char*)btSerializer->getBufferPointer(), btSerializer->getCurrentBufferSize());
}

} // namespace detail
} // namespace database
} // namespace slon
