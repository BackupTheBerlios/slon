#include "stdafx.h"
#include "PyGraphicsManager.h"
#include "Graphics/GraphicsManager.h"
#include <boost/python.hpp>

using namespace boost::python;
using namespace slon::graphics;

// wrappers
void setVideoMode( unsigned     width,
                   unsigned     height,
                   unsigned     bpp,
                   bool         fullscreen = false,
                   bool         vSync = false,
                   int          multisample = 0 )
{
    currentGraphicsManager().setVideoMode(width, height, bpp, fullscreen, vSync, multisample);
    currentGraphicsManager().initRenderer( FFPRendererDesc() );
}

void exportGraphicsManager()
{
    def("setVideoMode", setVideoMode);
}