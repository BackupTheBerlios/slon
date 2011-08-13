#include "stdafx.h"
#include "PyGraphicsManager.h"
#include "PyMesh.h"
#include "PySGL.h"

BOOST_PYTHON_MODULE(graphics)
{
    exportGraphicsManager();
    exportMesh();
    exportSGL();
}