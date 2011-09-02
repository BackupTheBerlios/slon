#include "stdafx.h"
#include "PyRigidBody.h"
#include "Physics/RigidBody.h"

using namespace boost::python;
using namespace slon::physics;

void exportRigidBody()
{
    scope rbScope =
        class_<RigidBody, bases<CollisionObject>, boost::intrusive_ptr<RigidBody>, boost::noncopyable>("RigidBody")
            .def(init<>())
            .def(init<const RigidBody::DESC&>())
            .def("applyForce",                  &RigidBody::applyForce)
            .def("applyTorque",                 &RigidBody::applyTorque)
            .def("applyImpulse",                &RigidBody::applyImpulse)
            .def("applyTorqueImpulse",          &RigidBody::applyTorqueImpulse)
            .add_property("totalForce",         &RigidBody::getTotalForce)
            .add_property("totalTorque",        &RigidBody::getTotalTorque)
            .add_property("mass",               &RigidBody::getMass)
            .add_property("inertiaTensor",      &RigidBody::getInertiaTensor)
            .add_property("activationState",    &RigidBody::getActivationState, &RigidBody::setActivationState)
            .add_property("linearVelocity",     &RigidBody::getLinearVelocity)
            .add_property("angularVelocity",    &RigidBody::getAngularVelocity)
            .add_property("dynamicsType",       &RigidBody::getDynamicsType)
            .def("getDesc",                     &RigidBody::getDesc, return_internal_reference<>())
            .def("reset",                       &RigidBody::reset);

    implicitly_convertible< boost::intrusive_ptr<RigidBody>,        boost::intrusive_ptr<CollisionObject> >();
    implicitly_convertible< boost::intrusive_ptr<RigidBody>,        boost::intrusive_ptr<const CollisionObject> >();
    implicitly_convertible< boost::intrusive_ptr<const RigidBody>,  boost::intrusive_ptr<const CollisionObject> >();

    class_<RigidBody::DESC, boost::shared_ptr<RigidBody::DESC>, boost::noncopyable>("DESC")
        .def(init<>())
        .def_readwrite("transform",       &RigidBody::DESC::transform)
        .def_readwrite("type",            &RigidBody::DESC::type)
        .def_readwrite("mass",            &RigidBody::DESC::mass)
        .def_readwrite("inertia",         &RigidBody::DESC::inertia) 
        .def_readwrite("margin",          &RigidBody::DESC::margin)
        .def_readwrite("relativeMargin",  &RigidBody::DESC::relativeMargin)
        .def_readwrite("linearVelocity",  &RigidBody::DESC::linearVelocity)
        .def_readwrite("angularVelocity", &RigidBody::DESC::angularVelocity)
        .def_readwrite("name",            &RigidBody::DESC::name)
        .def_readwrite("collisionShape",  &RigidBody::DESC::collisionShape);

    enum_<RigidBody::DYNAMICS_TYPE>("DYNAMICS_TYPE")
        .value("STATIC",    RigidBody::DT_STATIC)
        .value("KINEMATIC", RigidBody::DT_KINEMATIC)
        .value("DYNAMIC",   RigidBody::DT_DYNAMIC);

    enum_<RigidBody::ACTIVATION_STATE>("ACTIVATION_STATE")
        .value("ACTIVE",                RigidBody::AS_ACTIVE)
        .value("SLEEPING",              RigidBody::AS_SLEEPING)
        .value("DISABLE_DEACTIVATION",  RigidBody::AS_DISABLE_DEACTIVATION)
        .value("DISABLE_SIMULATION",    RigidBody::AS_DISABLE_SIMULATION);
}
