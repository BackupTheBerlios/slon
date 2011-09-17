#include "stdafx.h"
#include "PyAABB.h"
#include "PyPlane.h"
#include "PyMatrix.h"
#include "PyVector.h"

BOOST_PYTHON_MODULE(math)
{
    exportAABB();
    exportPlane();
    exportMatrix();
    exportVector();
}