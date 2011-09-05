#include "stdafx.h"
#include "PyConstraint.h"
#include "Physics/Constraint.h"
#include "Physics/ConstraintNode.h"

using namespace boost::python;
using namespace slon;
using namespace slon::physics;

// wrappers
typedef boost::shared_ptr<math::Matrix4r> matrix_ptr;
typedef boost::shared_ptr<math::Vector3r> vector_ptr;

template<typename T>
void no_delete(T*) {}

rigid_body_ptr  DESCgetRigidBodyA(Constraint::DESC& desc)                               { return desc.rigidBodies[0]; }
void            DESCsetRigidBodyA(Constraint::DESC& desc, const rigid_body_ptr& rb)     { desc.rigidBodies[0] = rb; }

rigid_body_ptr  DESCgetRigidBodyB(Constraint::DESC& desc)                               { return desc.rigidBodies[1]; }
void            DESCsetRigidBodyB(Constraint::DESC& desc, const rigid_body_ptr& rb)     { desc.rigidBodies[1] = rb; }

matrix_ptr      DESCgetFrameA(Constraint::DESC& desc)                                   { return matrix_ptr(&desc.frames[0], no_delete<math::Matrix4r>); }
void            DESCsetFrameA(Constraint::DESC& desc, const matrix_ptr& m)              { desc.frames[0] = *m; }

matrix_ptr      DESCgetFrameB(Constraint::DESC& desc)                                   { return matrix_ptr(&desc.frames[1], no_delete<math::Matrix4r>); }
void            DESCsetFrameB(Constraint::DESC& desc, const matrix_ptr& m)              { desc.frames[1] = *m; }

vector_ptr      DESCgetLinearLimitMin(Constraint::DESC& desc)                           { return vector_ptr(&desc.linearLimits[0], no_delete<math::Vector3r>); }
void            DESCsetLinearLimitMin(Constraint::DESC& desc, const math::Vector3f& v)  { desc.linearLimits[0] = v; }

vector_ptr      DESCgetLinearLimitMax(Constraint::DESC& desc)                           { return vector_ptr(&desc.linearLimits[1], no_delete<math::Vector3r>); }
void            DESCsetLinearLimitMax(Constraint::DESC& desc, const math::Vector3f& v)  { desc.linearLimits[1] = v; }

vector_ptr      DESCgetAngularLimitMin(Constraint::DESC& desc)                          { return vector_ptr(&desc.angularLimits[0], no_delete<math::Vector3r>); }
void            DESCsetAngularLimitMin(Constraint::DESC& desc, const math::Vector3f& v) { desc.angularLimits[0] = v; }

vector_ptr      DESCgetAngularLimitMax(Constraint::DESC& desc)                          { return vector_ptr(&desc.angularLimits[1], no_delete<math::Vector3r>); }
void            DESCsetAngularLimitMax(Constraint::DESC& desc, const math::Vector3f& v) { desc.angularLimits[1] = v; }

void exportConstraint()
{
    class_<ConstraintNode, bases<scene::Node>, boost::intrusive_ptr<ConstraintNode>, boost::noncopyable>("ConstraintNode")
        .def(init<>())
        .def(init<const constraint_ptr&>());
    implicitly_convertible< boost::intrusive_ptr<ConstraintNode>,       boost::intrusive_ptr<scene::Node> >();
    implicitly_convertible< boost::intrusive_ptr<ConstraintNode>,       boost::intrusive_ptr<const scene::Node> >();
    implicitly_convertible< boost::intrusive_ptr<const ConstraintNode>, boost::intrusive_ptr<const scene::Node> >();

    scope cScope =
        class_<Constraint, boost::intrusive_ptr<Constraint>, boost::noncopyable>("Constraint")
            .def(init<>())
            .def(init<const Constraint::DESC&>())
            .def("getAxis",         &Constraint::getAxis)
            .def("getPosition",     &Constraint::getPosition)
            .def("getRestriction",  &Constraint::getRestriction)
            .def("getName",         &Constraint::getName, return_internal_reference<>())
            .def("getDesc",         &Constraint::getDesc, return_internal_reference<>())
            .def("reset",           &Constraint::reset);

    class_<Constraint::DESC, boost::shared_ptr<Constraint::DESC>, boost::noncopyable>("DESC")
        .def(init<>())
        .def_readwrite("name",              &Constraint::DESC::name)
        .add_property("rigidBodyA",         make_function(DESCgetRigidBodyA, with_custodian_and_ward_postcall<1, 0>()),      DESCsetRigidBodyA)
        .add_property("rigidBodyB",         make_function(DESCgetRigidBodyB, with_custodian_and_ward_postcall<1, 0>()),      DESCsetRigidBodyB)
        .add_property("frameA",             make_function(DESCgetFrameA, with_custodian_and_ward_postcall<1, 0>()),          DESCsetFrameA)
        .add_property("frameB",             make_function(DESCgetFrameB, with_custodian_and_ward_postcall<1, 0>()),          DESCsetFrameB)
        .add_property("linearLimitMin",     make_function(DESCgetLinearLimitMin, with_custodian_and_ward_postcall<1, 0>()),  DESCsetLinearLimitMin)
        .add_property("linearLimitMax",     make_function(DESCgetLinearLimitMax, with_custodian_and_ward_postcall<1, 0>()),  DESCsetLinearLimitMax)
        .add_property("angularLimitMin",    make_function(DESCgetAngularLimitMin, with_custodian_and_ward_postcall<1, 0>()), DESCsetAngularLimitMin)
        .add_property("angularLimitMax",    make_function(DESCgetAngularLimitMax, with_custodian_and_ward_postcall<1, 0>()), DESCsetAngularLimitMax);

    enum_<Constraint::AXIS_RESTRICTION>("AXIS_RESTRICTION")
        .value("FREE",          Constraint::AXIS_FREE)
        .value("RESTRICTED",    Constraint::AXIS_RESTRICTED)
        .value("LOCKED",        Constraint::AXIS_LOCKED);

    enum_<Constraint::DOF>("DOF")
        .value("X_TRANS",   Constraint::DOF_X_TRANS)
        .value("Y_TRANS",   Constraint::DOF_Y_TRANS)
        .value("Z_TRANS",   Constraint::DOF_Z_TRANS)
        .value("X_ROT",     Constraint::DOF_X_ROT)
        .value("Y_ROT",     Constraint::DOF_Y_ROT)
        .value("Z_ROT",     Constraint::DOF_Z_ROT);
}