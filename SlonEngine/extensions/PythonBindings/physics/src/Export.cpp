#include "stdafx.h"
#include "PyCollisionObject.h"
#include "PyCollisionShape.h"
#include "PyConstraint.h"
#include "PyPhysicsTransform.h"
#include "PyRigidBody.h"

BOOST_PYTHON_MODULE(physics)
{
    exportCollisionObject();
    exportCollisionShape();
    exportConstraint();
    exportPhysicsTransform();
    exportRigidBody();
}