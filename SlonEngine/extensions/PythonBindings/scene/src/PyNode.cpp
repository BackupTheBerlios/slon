#include "stdafx.h"
#include "PyNode.h"
#include "Scene/Group.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

// wrappers
void        NodeSetName(Node* node, const std::string& name) { node->setName(name); }
std::string NodeGetName(const Node* node)                    { return node->getName().str(); }

Node*       (Node::*NodeGetLeft)()   = &Node::getLeft;
Node*       (Node::*NodeGetRight)()  = &Node::getRight;
Group*      (Node::*NodeGetParent)() = &Node::getParent;

void exportNode()
{
    class_<Node, boost::intrusive_ptr<Node>, boost::noncopyable>("Node", init<>())
        .add_property("name", NodeGetName, NodeSetName)
        .def("getLeft", NodeGetLeft, return_value_policy<reference_existing_object>())
        .def("getRight", NodeGetRight, return_value_policy<reference_existing_object>())
        .def("getParent", NodeGetParent, return_value_policy<reference_existing_object>());
}