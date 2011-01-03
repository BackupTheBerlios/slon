#include "stdafx.h"
#include "Database/Bullet/Bullet.h"
#include "Physics/PhysicsManager.h"
#include "Physics/PhysicsModel.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletDynamicsWorld.h"
#include "Physics/Bullet/BulletConstraint.h"
#include "Physics/Bullet/BulletRigidBody.h"
#include "Utility/error.hpp"
#include <Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

__DEFINE_LOGGER__("Database.Bullet")

namespace slon {
namespace db {
namespace detail {

void loadBulletPhysicsScene( const std::string&         fileName,
                             physics::PhysicsModel&     sceneModel,
                             physics::DynamicsWorld&    dynamicsWorld )
{
    physics::BulletDynamicsWorld& world = static_cast<physics::BulletDynamicsWorld&>(dynamicsWorld);
    
    std::auto_ptr<btBulletWorldImporter> fileLoader( new btBulletWorldImporter( &world.getBtDynamicsWorld() ) );
	if ( !fileLoader->loadFile( fileName.c_str() ) ) {
        throw file_not_found_error(logger, "Can't load bullet physics file: " + fileName);
    }

    // enumerate objects and add them to the scene model
    for (int i = 0; i<fileLoader->getNumRigidBodies(); ++i) 
    {
        btCollisionObject* collisionObject = fileLoader->getRigidBodyByIndex(i);
        if ( btRigidBody* rigidBody = dynamic_cast<btRigidBody*>(collisionObject) ) 
        {
            sceneModel.addRigidBody( new physics::BulletRigidBody( physics::BulletRigidBody::rigid_body_ptr(rigidBody), 
                                                                   fileLoader->getNameForPointer(rigidBody),
                                                                   &world ) );
        }
    }

    for (int i = 0; i<fileLoader->getNumConstraints(); ++i) 
    {
        physics::BulletConstraint::bullet_constraint_ptr constraint( (btGeneric6DofConstraint*)fileLoader->getConstraintByIndex(i) );
        sceneModel.addConstraint( new physics::BulletConstraint( constraint, fileLoader->getNameForPointer( constraint.get() ) ) );
    }
}

} // namespace detail
} // namespace db
} // namespace slon
