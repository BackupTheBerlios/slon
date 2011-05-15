#include "stdafx.h"
#include "Database/Collada/Collada.h"
#include "Detail/Engine.h"
#include "FileSystem/File.h"
#include "Graphics/Common.h"
#include "Scene/Camera.h"
#include "Scene/Visitors/TransformVisitor.h"
#include "Utility/error.hpp"
#include <boost/filesystem.hpp>
#include <SDL.h>
#ifdef SLON_ENGINE_USE_BULLET
#   include "Database/Bullet/Bullet.h"
#endif

__DEFINE_LOGGER__("engine")

namespace {

    using namespace sgl;
    using namespace slon;

    // Place physics and input handling in single thread
    struct simulation_thread :
        public std::unary_function<void, void>
    {
        simulation_thread( Engine& _engine,
                           int     _maxOverhead,
                           bool    _multithreaded) :
            engine(_engine),
            numFramesTraversed(0),
            maxOverhead(_maxOverhead),
            multithreaded(_multithreaded)
        {};

        void operator () (void)
        {
            realm::detail::World& world = static_cast<realm::detail::World&>(engine.getWorld());
            thread::detail::ThreadManager& threadManager = static_cast<thread::detail::ThreadManager&>(engine.getThreadManager());
			if (multithreaded)
			{
				while( engine.isRunning() )
				{
					// perform up to 10 delayed functions
					int i = 0;
                    while ( threadManager.performDelayedFunctions(thread::SIMULATION_THREAD) && ++i < 10 ) {}

					// physics simulation doesn't modify the world directly, but
					// we will get problems if world simulation and traverse overlap
					thread::lock_ptr lock = world.lockForReading();
				#ifdef SLON_ENGINE_USE_PHYSICS
                    static_cast<physics::detail::PhysicsManager&>( engine.getPhysicsManager() ).handlePhysics();
				#endif
				}
			}
            else
			{
				// perform up to 10 delayed functions
				int i = 0;
				while ( threadManager.performDelayedFunctions(thread::SIMULATION_THREAD) && ++i < 10 ) {}

				// physics simulation doesn't modify the world directly, but
				// we will get problems if world simulation and traverse overlap
				thread::lock_ptr lock = world.lockForReading();
			#ifdef SLON_ENGINE_USE_PHYSICS
				static_cast<physics::detail::PhysicsManager&>( engine.getPhysicsManager() ).handlePhysics();
			#endif
			}
        }

        Engine& engine;
        size_t  numFramesTraversed;
        int     maxOverhead;
        bool    multithreaded;
    };

    // handler for sgl errors
    class LogErrorHandler :
        public sgl::ReferencedImpl<sgl::ErrorHandler>
    {
    public:
        LogErrorHandler(const std::string& logName, bool breakOnError_)
        :   logger(logName)
		,	breakOnError(breakOnError_)
        {}

        // Override error handler
	    void HandleError(SGL_HRESULT result, const char* msg)
	    {
			if (breakOnError) {
				debug_break();
			}

		    switch(result)
		    {
		    case SGLERR_INVALID_CALL:
			    logger << log::S_ERROR << "Invalid call: " << msg << std::endl;
			    break;

		    case SGLERR_OUT_OF_MEMORY:
			    logger << log::S_ERROR << "Out of memory: " << msg << std::endl;
			    break;

		    case SGLERR_FILE_NOT_FOUND:
			    logger << log::S_ERROR << "File not found: " << msg << std::endl;
			    break;

		    case SGLERR_UNSUPPORTED:
			    logger << log::S_ERROR << "Unsupported function: " << msg << std::endl;
			    break;

		    default:
			    logger << log::S_ERROR << "Unknown error: " << msg << std::endl;
			    break;
		    }
	    }

    private:
        log::Logger logger;
		bool		breakOnError;
    };

    sgl::ref_ptr<LogErrorHandler> logErrorHandler;

    template<sgl::Image::FILE_TYPE format>
    class ImageLoader :
        public database::TextureLoader
    {
    public:
		bool binary() const { return true; }

        graphics::texture_ptr load(std::istream& stream)
        {
            graphics::texture_ptr texture;

			std::stringstream buffer;
			buffer << stream.rdbuf();
			std::string data( buffer.str() );

            graphics::image_ptr image( graphics::currentDevice()->CreateImage() );
		    if ( sgl::SGL_OK == image->LoadFromFileInMemory(format, data.size(), &data[0]) )
		    {
			    //if (image->Depth() == 6) {
			    //	texture = sgl::ref_ptr<sgl::Texture>( image->CreateTextureCube() );
			    //}
			    if (image->Depth() > 1) {
                    texture.reset( image->CreateTexture3D() );
			    }
			    else {
                    texture.reset( image->CreateTexture2D() );
			    }
			}
                
			return texture;
        }
    };

} // anonymous namespace

namespace slon {
namespace detail {

// static
Engine* Engine::engineInstance = 0;

Engine::Engine() :
    working(false)
{
    filesystemManager.reset(new filesystem::detail::FileSystemManager);
}

void Engine::init()
{
	namespace fs = boost::filesystem;

	// Init filesystem
    filesystemManager->mount( fs::system_complete( fs::current_path() ).file_string().c_str(), "/" );

    // Setup error logger
    logErrorHandler.reset( new LogErrorHandler("graphics.sgl", true) );
    sglSetErrorHandler( logErrorHandler.get() );

    // redirect loggers
    logManager.redirectOutput("", "log.txt");
    logManager.redirectOutputToConsole("database");
    logManager.redirectOutputToConsole("graphics");

    // initialize loaders
    {
        using namespace database::detail;

        const size_t                    numLibraryLoaders = 1;
        fmt_loader<database::Library>   libraryLoaders[numLibraryLoaders] = 
        {
            {"COLLADA", 2, {".*\\.(?i:dae)", ".*"}, new database::ColladaLoader}
        };
        database::detail::registerLoaders<database::Library>(numLibraryLoaders, libraryLoaders);
    
        const size_t                    numImageFormats = 11;
        fmt_loader<graphics::Texture>   imageLoaders[numImageFormats] =
        {
            { "BMP",    2, {".*\\.(?i:bmp)", ".*"},     new ImageLoader<sgl::Image::BMP>},
            { "DDS",    2, {".*\\.(?i:dds)", ".*"},     new ImageLoader<sgl::Image::DDS>},
            { "GIF",    2, {".*\\.(?i:gif)", ".*"},     new ImageLoader<sgl::Image::GIF>},
            { "ICO",    2, {".*\\.(?i:ico)", ".*"},     new ImageLoader<sgl::Image::ICO>},
            { "HDR",    2, {".*\\.(?i:hdr)", ".*"},     new ImageLoader<sgl::Image::HDR>},
            { "JPEG",   2, {".*\\.(?i:jpe?g)", ".*"},   new ImageLoader<sgl::Image::JPG>},
            { "PIC",    2, {".*\\.(?i:pic)", ".*"},     new ImageLoader<sgl::Image::PIC>},
            { "PNG",    2, {".*\\.(?i:png)", ".*"},     new ImageLoader<sgl::Image::PNG>},
            { "PSD",    2, {".*\\.(?i:psd)", ".*"},     new ImageLoader<sgl::Image::PSD>},
            { "TGA",    2, {".*\\.(?i:tga)", ".*"},     new ImageLoader<sgl::Image::TGA>},
            { "TIF",    2, {".*\\.(?i:tiff?)", ".*"},   new ImageLoader<sgl::Image::TIF>}
        };
        database::detail::registerLoaders<graphics::Texture>(numImageFormats, imageLoaders);

#ifdef SLON_ENGINE_USE_BULLET
        const size_t                      numPhysicsSceneLoaders = 1;
        fmt_loader<physics::PhysicsModel> physicsSceneLoaders[numPhysicsSceneLoaders] = 
        {
            {"BULLET",  2, {".*\\.(?i:bullet)", ".*"}, new database::detail::BulletLoader}
        };
        database::detail::registerLoaders<physics::PhysicsModel>(numPhysicsSceneLoaders, physicsSceneLoaders);

        const size_t                      numPhysicsSceneSavers = 1;
        fmt_saver<physics::PhysicsModel>  physicsSceneSavers[numPhysicsSceneSavers] = 
        {
            {"BULLET",  2, {".*\\.(?i:bullet)", ".*"}, new database::detail::BulletSaver}
        };
        database::detail::registerSavers<physics::PhysicsModel>(numPhysicsSceneSavers, physicsSceneSavers);
#endif
    }

    // init SDL
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 && !SDL_GetVideoInfo() ) {
        throw slon_error(logger, "Can't init SDL");
    }

    // setup timer for PhysicsManager
    simulationTimer.reset(new StartStopTimer);
#ifdef SLON_ENGINE_USE_PHYSICS
    physicsManager.setTimer( simulationTimer.get() );
#endif
}

void Engine::run(const DESC& desc_)
{
    desc           = desc_;
    frameNumber    = 0;
    working        = true;

    // clear event queue before start
    while ( !SDL_PollEvent(0) ) {}
    if (desc.grabInput) {
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }

    // run simulation thread
#ifdef SLON_ENGINE_USE_PHYSICS
    simulation_thread simulationThread(*this, 5, desc.multithreaded);
    if (desc.multithreaded) {
        threadManager.startThread(thread::SIMULATION_THREAD, simulationThread);
    }
#endif

    // run main rendering cycle
    scene::TransformVisitor traverser;
    simulationTimer->start();
    while (working)
    {
        ++frameNumber;

        // handle physics if not multithreaded
#ifdef SLON_ENGINE_USE_PHYSICS
        if (!desc.multithreaded) {
            simulationThread();
        }
#endif
        // handle input
        inputManager.handleEvents();

        // perform 10 delayed functions
        int i = 0;
        while ( threadManager.performDelayedFunctions(thread::MAIN_THREAD) && ++i < 10 ) {}

        // traverse updated objects
        {
            thread::lock_ptr lock = world.lockForWriting();
            for (size_t i = 0; i<updateQueue.size(); ++i) 
            {
                updateQueue[i]->traverse(traverser);
                world.update(updateQueue[i].get());
            }
            updateQueue.clear();
        }
        graphicsManager.render(world);
    }

    // remove useless now delegates
    // threadManager.clearDelegates();

    if (desc.grabInput) {
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }

    // wake input thread
    SDL_Event dummy;    
    SDL_PushEvent(&dummy);

    // join simulation thread
    threadManager.joinThread(thread::SIMULATION_THREAD);

    sglSetErrorHandler(0);
    SDL_Quit();
}


void Engine::frame()
{
    bool fullFrame = !working;
    if (fullFrame)
    {
        working = true;
        simulationTimer->start();
    }

#ifdef SLON_ENGINE_USE_PHYSICS
    simulation_thread simulation(*this, 5, false);
    simulation();
#endif
    // handle input
    inputManager.handleEvents();

    // perform 10 delayed functions
    int i = 0;
    scene::TransformVisitor traverser;    
    while ( threadManager.performDelayedFunctions(thread::MAIN_THREAD) && ++i < 10 ) {}

        // traverse updated objects
    {
        thread::lock_ptr lock = world.lockForWriting();
        for (size_t i = 0; i<updateQueue.size(); ++i) 
        {
            updateQueue[i]->traverse(traverser);
            world.update(updateQueue[i].get());
        }
        updateQueue.clear();
    }
    graphicsManager.render(world);

    ++frameNumber;

    if (fullFrame) {    
        working = false;
    }
}

Engine::~Engine()
{
}

} // namespace detail

Engine* Engine::Instance()
{
    if (!detail::Engine::engineInstance) {
        detail::Engine::engineInstance = new detail::Engine();
    }

    return detail::Engine::engineInstance;
}

void Engine::Free()
{
    delete detail::Engine::engineInstance;
#ifdef _DEBUG
    freopen("leaks.txt", "w", stderr);
    check_mem_corruption();
    check_leaks();
#endif
}

// Define manager get funcs
input::InputManager& input::currentInputManager()
{
    return Engine::Instance()->getInputManager();
}

graphics::GraphicsManager& graphics::currentGraphicsManager()
{
    return Engine::Instance()->getGraphicsManager();
}

thread::ThreadManager& thread::currentThreadManager()
{
    return Engine::Instance()->getThreadManager();
}

#ifdef SLON_ENGINE_USE_PHYSICS
physics::PhysicsManager& physics::currentPhysicsManager()
{
	return Engine::Instance()->getPhysicsManager();
}
#endif

} // namespace slon
