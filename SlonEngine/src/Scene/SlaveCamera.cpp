#include "stdafx.h"
#include "Scene/SlaveCamera.h"

using namespace slon;
using namespace scene;

SlaveCamera::SlaveCamera()
{
}

SlaveCamera::SlaveCamera(const Camera& _masterCamera) :
    masterCamera(&_masterCamera)
{
}
