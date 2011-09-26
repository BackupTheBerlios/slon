#include "stdafx.h"
#include "Scene/Skeleton.h"
#include <sgl/Math/Matrix.hpp>

namespace {

} // anonymous namespace

namespace slon {
namespace scene {

void Skeleton::setRootJoint(Joint* _root) 
{ 
    root.reset(_root);
}

} // namespace scene
} // namespace slon
