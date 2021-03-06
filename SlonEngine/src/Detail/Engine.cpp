#include "stdafx.h"
#include "Database/Collada/Collada.h"
#include "Database/Proprietary/SXMLLoader.h"
#include "Database/Proprietary/SXMLSaver.h"
#include "Detail/Engine.h"
#include "FileSystem/File.h"
#include "Graphics/Common.h"
#include "Graphics/LightingEffect.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/StaticMesh.h"
#include "Physics/CollisionShape.h"
#include "Physics/Constraint.h"
#include "Physics/ConstraintNode.h"
#include "Physics/RigidBody.h"
#include "Physics/PhysicsTransform.h"
#include "Realm/BVHLocation.h"
#include "Realm/DefaultWorld.h"
#include "Scene/Camera.h"
#include "Scene/TransformVisitor.h"
#include "Utility/error.hpp"
#include <boost/filesystem.hpp>
#include <SDL.h>
#ifdef SLON_ENGINE_USE_BULLET
#   include "Database/Bullet/Bullet.h"
#endif

DECLARE_AUTO_LOGGER("Engine")

namespace {

    using namespace sgl;
    using namespace slon;

    // handler for sgl errors
    class LogErrorHandler :
		public sgl::ErrorHandler
    {
    public:
        LogErrorHandler(log::LogManager& lm, const std::string& logName, bool breakOnError_)
        :   logger( lm.createLogger(logName) )
		,	breakOnError(breakOnError_)
        {}

        // Override error handler
	    void HandleError(SGL_HRESULT result, const char* msg)
	    {
		    switch(result)
		    {
		    case SGLERR_INVALID_CALL:
			    (*logger) << log::S_ERROR << "Invalid call: " << msg << std::endl;
			    break;

		    case SGLERR_OUT_OF_MEMORY:
			    (*logger) << log::S_ERROR << "Out of memory: " << msg << std::endl;
			    break;

		    case SGLERR_FILE_NOT_FOUND:
			    (*logger) << log::S_ERROR << "File not found: " << msg << std::endl;
			    break;

		    case SGLERR_UNSUPPORTED:
			    (*logger) << log::S_ERROR << "Unsupported function: " << msg << std::endl;
			    break;

		    default:
			    (*logger) << log::S_ERROR << "Unknown error: " << msg << std::endl;
			    break;
		    }

			if (breakOnError) {
				debug_break();
			}
	    }

    private:
        log::logger_ptr logger;
		bool			breakOnError;
    };

    LogErrorHandler* logErrorHandler;

    template<sgl::Image::FILE_TYPE format>
    class ImageLoader :
        public database::TextureLoader
    {
    public:
		bool binary() const { return true; }

        graphics::texture_ptr load(filesystem::File* file)
        {
            graphics::texture_ptr texture;

            file->open(filesystem::File::in | filesystem::File::binary);
			std::string data(file->size(), ' ');
            file->read(&data[0], data.size());
            file->close();

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
    // init world
    world.reset(new realm::DefaultWorld);

	namespace fs = boost::filesystem;

	// Init filesystem
    filesystemManager.reset(new filesystem::detail::FileSystemManager);
    filesystemManager->mount( fs::system_complete( fs::current_path() ).file_string().c_str(), "/" );

    // Setup error logger
    logErrorHandler = new LogErrorHandler(logManager, "graphics.sgl", true);
    sglSetErrorHandler(logErrorHandler);

    // redirect loggers
    logManager.redirectOutput("", "log.txt");
    logManager.redirectOutputToConsole("database");
    logManager.redirectOutputToConsole("graphics");

    // initialize loaders & savers
    {
        using namespace database::detail;

        const size_t                    numLibraryLoaders = 2;
        fmt_loader<database::Library>   libraryLoaders[numLibraryLoaders] = 
        {
            {"COLLADA", 2, {".*\\.(?i:dae)", ".*"}, new database::ColladaLoader},
            {"SXML", 2, {".*\\.(?i:sxml)", ".*"}, new database::SXMLLoader}
        };
        database::detail::registerLoaders<database::Library>(databaseManager, numLibraryLoaders, libraryLoaders);

        const size_t                    numLibrarySavers = 1;
        fmt_saver<database::Library>    librarySavers[numLibrarySavers] = 
        {
            {"SXML", 2, {".*\\.(?i:sxml)", ".*"}, new database::SXMLSaver}
        };
        database::detail::registerSavers<database::Library>(databaseManager, numLibrarySavers, librarySavers);
    
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
        database::detail::registerLoaders<graphics::Texture>(databaseManager, numImageFormats, imageLoaders);

#ifdef SLON_ENGINE_USE_BULLET
        const size_t                      numPhysicsSceneLoaders = 1;
        fmt_loader<physics::PhysicsModel> physicsSceneLoaders[numPhysicsSceneLoaders] = 
        {
            {"BULLET",  2, {".*\\.(?i:bullet)", ".*"}, new database::detail::BulletLoader}
        };
        database::detail::registerLoaders<physics::PhysicsModel>(databaseManager, numPhysicsSceneLoaders, physicsSceneLoaders);

        const size_t                      numPhysicsSceneSavers = 1;
        fmt_saver<physics::PhysicsModel>  physicsSceneSavers[numPhysicsSceneSavers] = 
        {
            {"BULLET",  2, {".*\\.(?i:bullet)", ".*"}, new database::detail::BulletSaver}
        };
        database::detail::registerSavers<physics::PhysicsModel>(databaseManager, numPhysicsSceneSavers, physicsSceneSavers);
#endif
    }

	// register serializables
	{
		using namespace database;

        // scene
		databaseManager.registerSerializableCreateFunc("Node",                  createSerializable<scene::Node>);
		databaseManager.registerSerializableCreateFunc("Group",                 createSerializable<scene::Group>);
		databaseManager.registerSerializableCreateFunc("MatrixTransform",       createSerializable<scene::MatrixTransform>);
		databaseManager.registerSerializableCreateFunc("PhysicsTransform",      createSerializable<physics::PhysicsTransform>);
		databaseManager.registerSerializableCreateFunc("StaticMesh",            createSerializable<graphics::StaticMesh>);
		databaseManager.registerSerializableCreateFunc("GPUSideMesh",           createSerializable<graphics::GPUSideMesh>);

        // physics
#ifdef SLON_ENGINE_USE_PHYSICS
        {
            databaseManager.registerSerializableCreateFunc("PhysicsTransform",  createSerializable<physics::PhysicsTransform>);
            databaseManager.registerSerializableCreateFunc("RigidBody",         createSerializable<physics::RigidBody>);
            databaseManager.registerSerializableCreateFunc("ConstraintNode",    createSerializable<physics::ConstraintNode>);  
            databaseManager.registerSerializableCreateFunc("Constraint",        createSerializable<physics::Constraint>);           
            databaseManager.registerSerializableCreateFunc("PlaneShape",        createSerializable<physics::PlaneShape>);
            databaseManager.registerSerializableCreateFunc("SphereShape",       createSerializable<physics::SphereShape>);
            databaseManager.registerSerializableCreateFunc("BoxShape",          createSerializable<physics::BoxShape>);
            databaseManager.registerSerializableCreateFunc("ConeShape",         createSerializable<physics::ConeShape>);
            databaseManager.registerSerializableCreateFunc("CapsuleShape",      createSerializable<physics::CapsuleShape>);
            databaseManager.registerSerializableCreateFunc("CylinderXShape",    createSerializable<physics::CylinderXShape>);
            databaseManager.registerSerializableCreateFunc("CylinderYShape",    createSerializable<physics::CylinderYShape>);
            databaseManager.registerSerializableCreateFunc("CylinderZShape",    createSerializable<physics::CylinderZShape>);
            databaseManager.registerSerializableCreateFunc("ConvexShape",       createSerializable<physics::ConvexShape>);
            databaseManager.registerSerializableCreateFunc("TriangleMeshShape", createSerializable<physics::TriangleMeshShape>);
            databaseManager.registerSerializableCreateFunc("CompoundShape",     createSerializable<physics::CompoundShape>);
        }
#endif
        // realm
        databaseManager.registerSerializableCreateFunc("BVHLocation",           createSerializable<realm::BVHLocation>);
        databaseManager.registerSerializableCreateFunc("BVHLocationNode",       createSerializable<realm::BVHLocationNode>);

        // sgl
		databaseManager.registerSerializableCreateFunc("VertexLayout",          createSerializableWrapper<sgl::VertexLayout>);
		databaseManager.registerSerializableCreateFunc("VertexBuffer",          createSerializableWrapper<sgl::VertexBuffer>);
		databaseManager.registerSerializableCreateFunc("IndexBuffer",           createSerializableWrapper<sgl::IndexBuffer>);
        
        // graphics
		databaseManager.registerSerializableCreateFunc("LightingMaterial",      createSerializable<graphics::LightingMaterial>);
		databaseManager.registerSerializableCreateFunc("LightingEffect",        createSerializable<graphics::LightingEffect>);
	}

    // init SDL
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 && !SDL_GetVideoInfo() ) {
        throw slon_error(AUTO_LOGGER, "Can't init SDL");
    }

    // setup timer for PhysicsManager
    simulationTimer.reset(new StartStopTimer);
#ifdef SLON_ENGINE_USE_PHYSICS
    physicsManager.setTimer( simulationTimer.get() );
#endif
}

void Engine::handleInput()
{
    inputManager.handleEvents();
}

void Engine::handlePhysics()
{
    physicsManager.handlePhysics();
}

void Engine::handlePhysicsCycle()
{
    while (working)
    {
        thread::lock_ptr lock = physicsManager.lockForReading();
        handlePhysics();
    }
}

void Engine::handleGraphics()
{
	thread::lock_ptr lock = world->lockForReading();
    graphicsManager.render(*world);
}

void Engine::handleScene()
{
	thread::lock_ptr lock = world->lockForWriting();
    {
        boost::lock_guard<boost::mutex> lock(updateQueueMutex);
        updateQueueTemp.swap(updateQueue);
    }
   
    for (size_t i = 0; i<updateQueueTemp.size(); ++i)
    {
        if (updateQueueTemp[i]->updatedFrameNo < frameNumber)
        {
            updateQueueTemp[i]->onUpdate();
            updateQueueTemp[i]->updatedFrameNo = frameNumber;
        }
    }
    updateQueueTemp.clear();
}

void Engine::addToUpdateQueue(scene::Node* node)
{
    boost::lock_guard<boost::mutex> lock(updateQueueMutex);
    updateQueue.push_back(node);
}

void Engine::run(const DESC& desc_)
{
    desc        = desc_;
    frameNumber = 0;
    working     = true;

    // clear event queue before start
    while ( !SDL_PollEvent(0) ) {}
    if (desc.grabInput) {
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }

    // run simulation thread
#ifdef SLON_ENGINE_USE_PHYSICS
    if (desc.multithreaded) {
        threadManager.startThread(thread::SIMULATION_THREAD, boost::bind(&Engine::handlePhysicsCycle, this));
    }
#endif

    // run main rendering cycle
    simulationTimer->start();
    while (working) {
        frame();
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
#ifdef SLON_ENGINE_USE_PHYSICS
    if (desc.multithreaded) {
        threadManager.joinThread(thread::SIMULATION_THREAD);
    }
#endif
}


void Engine::frame()
{
    ++frameNumber;
    handleInput();
    if (!desc.multithreaded) {
        handlePhysics();
    }
    handleScene();
    handleGraphics();
}

Engine::~Engine()
{
    SDL_Quit();
	sglSetErrorHandler(0);
	delete logErrorHandler;
}

Engine* Engine::Instance()
{
    if (!detail::Engine::engineInstance) {
        detail::Engine::engineInstance = new detail::Engine();
    }

    return detail::Engine::engineInstance;
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
    detail::Engine::engineInstance = 0;
#ifdef SLON_ENGINE_FORCE_DEBUG_NEW
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
