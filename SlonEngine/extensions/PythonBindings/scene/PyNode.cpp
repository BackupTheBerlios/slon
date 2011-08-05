#include "stdafx.h"
#include "Scene/Node.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::scene;

BOOST_PYTHON_MODULE(scene)
{
    class_<Node, boost::intrusive_ptr<Node>, boost::noncopyable>("Node", init<>())
        .add_property("name", &Node::getName, &Node::setName);
}