#include "stdafx.h"
#include "Graphics/Common.h"
#include "Graphics/Detail/GraphicsManager.h"
#include "Graphics/Renderer/FixedPipelineRenderer.h"
#include "Graphics/Renderer/ForwardRenderer.h"
#include "Utility/error.hpp"
#include <SDL.h>
#if defined(__linux__)
#   include <GL/glx.h>
#endif

namespace slon {
namespace graphics {
namespace detail {

__DEFINE_LOGGER__("graphics")

GraphicsManager::GraphicsManager() :
    attributeTable(new detail::AttributeTable),
    parameterTable(new detail::ParameterTable),
    uniformTable(new detail::UniformTable)
{
}

void GraphicsManager::setVideoMode( unsigned     width,
                                    unsigned     height,
                                    unsigned     bpp,
                                    bool         fullscreen,
                                    bool         vSync,
                                    int			 multisample )
{
    // vSync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vSync);

    // misc
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // multisampling
    if ( multisample > 0 )
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
    }

    // create window
    unsigned flags = SDL_HWSURFACE | SDL_OPENGL;
    flags |= fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;

    if ( !SDL_SetVideoMode(width, height, bpp, flags) ) {
        throw slon_error(logger, (std::string("Couldn't set video mode: ") + SDL_GetError()).c_str() );
    }

#ifdef _WIN32
    FreeConsole();
#endif
}

graphics::Renderer* GraphicsManager::initRenderer(const ForwardRendererDesc& desc)
{
    for (int i = sgl::DV_OPENGL_3_2; i >= sgl::DV_OPENGL_2_1; --i)
    {
        device.reset( sglCreateDeviceFromCurrent( sgl::DEVICE_VERSION(i) ) );
        if (device) {
            break;
        }
    }

    if (!device) {
        throw slon_error(logger, "Can't initialize necessary device");
    }

    bitsPerPixel = desc.bitsPerPixel;
    depthBits    = desc.depthBits;
    multisample  = desc.multisample;
    stencilBits  = 8;

    renderer.reset( new detail::ForwardRenderer(desc) );
    return renderer.get();
}

graphics::Renderer* GraphicsManager::initRenderer(const FFPRendererDesc& desc)
{
    for (int i = sgl::DV_OPENGL_2_1; i >= 0; --i)
    {
        device.reset( sglCreateDeviceFromCurrent( sgl::DEVICE_VERSION(i) ) );
        if (device) {
            break;
        }
    }

    if (!device) {
        throw slon_error(logger, "Can't initialize necessary device");
    }

    bitsPerPixel = desc.bitsPerPixel;
    depthBits    = desc.depthBits;
    multisample  = desc.multisample;
    stencilBits  = 8;

    renderer.reset( new detail::FixedPipelineRenderer(desc) );
    return renderer.get();
}

void GraphicsManager::setRenderingSurface(Surface& surface)
{
#ifdef _WIN32
    assert(surface.hWnd && "GraphicsManager::setRenderingSurface - surface.hWnd may not be NULL");

    if (!surface.hDC)
    {
        surface.hDC = GetDC(surface.hWnd);

        PIXELFORMATDESCRIPTOR pfd;
        {
            ZeroMemory( &pfd, sizeof(pfd) );
            pfd.nSize       = sizeof(pfd);
            pfd.nVersion    = 1;
            pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType  = PFD_TYPE_RGBA;
            pfd.cColorBits  = 32;
            pfd.cDepthBits  = 16;
            pfd.iLayerType  = PFD_MAIN_PLANE;
        }

        int iFormat = ChoosePixelFormat(surface.hDC, &pfd);
        if (iFormat == 0) {
            throw system_error( logger, "GraphicsManager::setRenderingSurface - ChoosePixelFormat failed: ", GetLastError() );
        }

        if ( !SetPixelFormat( surface.hDC, iFormat, &pfd ) ) {
            throw system_error( logger, "GraphicsManager::setRenderingSurface - SetPixelFormat failed: ", GetLastError() );
        }
    }

    if (!surface.hRC)
    {
        surface.hRC = wglCreateContext(surface.hDC);
        if (!surface.hRC) {
            throw system_error( logger, "GraphicsManager::setRenderingSurface - wglCreateContext failed:", GetLastError() ); 
        }
    }

    if ( !wglMakeCurrent(surface.hDC, surface.hRC) ) {
        throw system_error( logger, "GraphicsManager::setRenderingSurface - wglMakeCurrent failed: ", GetLastError() ); 
    }
#elif defined(__linux__)
    assert(surface.drawable && "GraphicsManager::setRenderingSurface - surface.drawable may not be NULL");

    if (!surface.context)
    {
        int Attr[] = { GLX_RGBA,
                       GLX_RED_SIZE,        1,
                       GLX_GREEN_SIZE,      1,
                       GLX_BLUE_SIZE,       1,
                       GLX_ALPHA_SIZE,      1,
                       GLX_DEPTH_SIZE,      depthBits,
                       GLX_STENCIL_SIZE,    8,
                       GLX_DOUBLEBUFFER,
                       None };

        XVisualInfo* xvVisualInfo = glXChooseVisual(surface.display, DefaultScreen(surface.display), Attr);
        if (!xvVisualInfo) {
            throw slon_error(logger, "GraphicsManager::setRenderingSurface failed. Failed to choose visual info.");
        }

        surface.context = glXCreateContext(surface.display, xvVisualInfo, None, True);
        if (!surface.context) {
            throw slon_error(logger, "GraphicsManager::setRenderingSurface failed. Failed to create glx context");
        }
    }

    if ( !glXMakeCurrent(surface.display, surface.drawable, surface.context) )
    {
        // TODO: error report
        throw slon::slon_error(logger, "GraphicsManager::setRenderingSurface failed. glxMakeCurrent failed: ");
    }
#endif
    // create device
    if (!device)
    {
        for (int i = sgl::DV_OPENGL_3_2; i >= 0; --i)
        {
            device.reset( sglCreateDeviceFromCurrent( sgl::DEVICE_VERSION(i) ) );
            if (device) {
                break;
            }
        }
    }

    if (!device) {
        throw slon_error(logger, "GraphicsManager::setRenderingSurface failed. Failed to create sgl device");
    }
}

const Surface& GraphicsManager::resolveRenderingSurface()
{
#ifdef _WIN32
    surface.hWnd = GetForegroundWindow();
    surface.hDC  = wglGetCurrentDC();
    surface.hRC  = wglGetCurrentContext();
#elif defined(__linux__)
    surface.display  = glXGetCurrentDisplay();
    surface.drawable = glXGetCurrentDrawable();
    surface.context  = glXGetCurrentContext();
#endif
    return surface;
}

const Surface& GraphicsManager::getRenderingSurface() const
{
    return surface;
}

void GraphicsManager::render(realm::World& world)
{
    // render
    device->Clear(true, true, true);
    preFrameRenderSignal();
    for(size_t i = 0; i<cameras.size(); ++i) {
        renderer->render(world, *cameras[i]);
    }
    postFrameRenderSignal();
    device->SwapBuffers();
}

bool GraphicsManager::removeCamera(scene::Camera* camera)
{
    camera_iterator iter = std::remove( cameras.begin(), cameras.end(), scene::camera_ptr(camera) );
    if ( iter != cameras.end() ) 
    {
        cameras.erase(iter);
        return true;
    }

    return false;
}

} // namespace detail

Renderer* currentRenderer()
{
    return currentGraphicsManager().getRenderer();
}

} // namespace graphics
} // namespace slon
