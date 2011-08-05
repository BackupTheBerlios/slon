#include "stdafx.h"
#include "PyNode.h"
#include "PyGroup.h"

BOOST_PYTHON_MODULE(scene)
{
    exportNode();
    exportGroup();
}