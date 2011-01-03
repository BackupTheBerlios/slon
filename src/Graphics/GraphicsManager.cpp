#include "Engine.h"
#include "Graphics/Common.h"
#include "Scene/Visitors/TraverseVisitor.h"
#include "Utility/error.hpp"
#include <SDL.h>

using namespace slon;
using namespace graphics;

__DEFINE_LOGGER__("graphics")

GraphicsManager::GraphicsManager() :
    fixedPipeline(false),
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

    // grab context
    for (int i = sgl::DV_OPENGL_3_2; i >= 0; --i) 
    {
        device.reset( sglCreateDeviceFromCurrent( sgl::DEVICE_VERSION(i) ) );
        if (device) {
            break;
        }
    }

    if (!device) {
        throw slon_error(logger, "Couldn't grab current context by the device");
    }
}

void GraphicsManager::setRenderer(Renderer* renderer_)
{
    renderer.reset(renderer_);
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

GraphicsManager& slon::graphics::currentGraphicsManager()
{
    return Engine::Instance()->getGraphicsManager();
}

Renderer* slon::graphics::currentRenderer()
{
    return currentGraphicsManager().getRenderer();
}
