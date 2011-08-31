#include "stdafx.h"
#include "PyAABB.h"
#include "PyMatrix.h"
#include "PyVector.h"

BOOST_PYTHON_MODULE(math)
{
    exportAABB();
    exportMatrix();
    exportVector();
}