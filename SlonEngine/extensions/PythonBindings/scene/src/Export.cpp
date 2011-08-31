#include "stdafx.h"
#include "PyNode.h"
#include "PyGroup.h"
#include "PyMatrixTransform.h"
#include "PyTransform.h"

BOOST_PYTHON_MODULE(scene)
{
    exportNode();
    exportGroup();
    exportMatrixTransform();
    exportTransform();
}