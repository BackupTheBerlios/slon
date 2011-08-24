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

    enum_<PRIMITIVE_TYPE>("PRIMITIVE_TYPE")
        .value("POINTS",            POINTS)
        .value("LINES",             LINES)
        .value("LINE_STRIP",        LINE_STRIP)
        .value("LINE_LOOP",         LINE_LOOP)
        .value("TRIANGLES",         TRIANGLES)
        .value("TRIANGLE_STRIP",    TRIANGLE_STRIP)
        .value("TRIANGLE_FAN",      TRIANGLE_FAN)
        .value("QUADS",             QUADS)
        .value("QUAD_STRIP",        QUAD_STRIP)
        .value("POLYGON",           POLYGON);
}