#include "stdafx.h"
#include "PyContainers.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(common)
{
    exportContainers();
}
