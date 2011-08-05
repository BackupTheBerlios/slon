#include "stdafx.h"
#include "Scene/Node.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

// wrappers
void        NodeSetName(Node* node, const std::string& name) { node->setName(name); }
std::string NodeGetName(const Node* node)                    { return node->getName().str(); }

BOOST_PYTHON_MODULE(slon)
{
    class_<Node, boost::intrusive_ptr<Node>, boost::noncopyable>("Node", init<>())
        .add_property("name", NodeGetName, NodeSetName);
}