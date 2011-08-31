#include "stdafx.h"
#include "PyTransform.h"
#include "Scene/Transform.h"

using namespace boost::python;
using namespace slon::scene;

void exportTransform()
{
    class_<Transform, bases<Group>, boost::intrusive_ptr<Transform>, boost::noncopyable>("Transform", no_init); 
    
    implicitly_convertible< boost::intrusive_ptr<Transform>, boost::intrusive_ptr<Group> >();
    implicitly_convertible< boost::intrusive_ptr<Transform>, boost::intrusive_ptr<Node> >();
}