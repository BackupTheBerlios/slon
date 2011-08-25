#include "stdafx.h"
#include "PyMatrix.h"
#include "PyVector.h"

BOOST_PYTHON_MODULE(math)
{
    exportMatrix();
    exportVector();
}