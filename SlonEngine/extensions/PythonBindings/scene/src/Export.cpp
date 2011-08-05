#include "stdafx.h"
#include "PyNode.h"
#include "PyGroup.h"
#include "PyMatrixTransform.h"

BOOST_PYTHON_MODULE(scene)
{
    exportNode();
    exportGroup();
    exportMatrixTransform();
}