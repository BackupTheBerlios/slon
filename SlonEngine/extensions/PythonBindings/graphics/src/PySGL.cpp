#include "stdafx.h"
#include "Graphics/Common.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace sgl;

// wrappers

void exportSGL()
{
    enum_<SCALAR_TYPE>("SCALAR_TYPE")
        .value("UNKNOWN", UNKNOWN)
        .value("BIT",     BIT)
        .value("BOOL",    BOOL)
        .value("BYTE",    BYTE)
        .value("UBYTE",   UBYTE)
        .value("SHORT",   SHORT)
        .value("USHORT",  USHORT)
        .value("INT",     INT)
        .value("UINT",    UINT)
        .value("FLOAT",   FLOAT)
        .value("DOUBLE",  DOUBLE);
}