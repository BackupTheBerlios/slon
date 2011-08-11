#include "stdafx.h"
#include "PyGroup.h"
#include "Scene/Group.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

// wrappers
void    GroupAddChildDef1(Group* group, Node* child) { group->addChild(child); }
void    GroupAddChildDef2(Group* group, const node_ptr& child) { group->addChild(child); }

Node*   (Group::*GroupGetChild)() = &Group::getChild;

void exportGroup()
{
    class_<Group, bases<Node>, boost::intrusive_ptr<Group>, boost::noncopyable>("Group", init<>())
        .def("addChild", &Group::addChild)
        .def("addChild", GroupAddChildDef1)
        .def("addChild", GroupAddChildDef2)
        .def("getChild", GroupGetChild, return_value_policy<reference_existing_object>());
}