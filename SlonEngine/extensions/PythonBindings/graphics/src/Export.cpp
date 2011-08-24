#include "stdafx.h"
#include "PyEffect.h"
#include "PyGraphicsManager.h"
#include "PyMesh.h"
#include "PySGL.h"

BOOST_PYTHON_MODULE(graphics)
{
    exportEffects();
    exportGraphicsManager();
    exportMesh();
    exportSGL();
}