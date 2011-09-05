#ifndef __SLON_ENGINE_ENGINE_H__
#define __SLON_ENGINE_ENGINE_H__

#include "Config.h"
#include "Animation/Animation.h"
#include "Input/InputManager.h"
#include "Database/DatabaseManager.h"
#include "FileSystem/FileSystemManager.h"
#include "Graphics/GraphicsManager.h"
#include "Log/LogManager.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "Physics/PhysicsManager.h"
#endif
#include "Realm/World.h"
#include "Thread/ThreadManager.h"
#include "Thread/Timer.h"

namespace slon {

/** Main class of the SlonEngine. The manager of the hole graphics,
 * physics, sound and etc in the scene.
 */
class SLON_PUBLIC Engine
{
public:
    struct DESC
    {
        bool multithreaded;
        bool grabInput;

        DESC() :
            multithreaded(false),
            grabInput(true)
        {}
    };

public:
    /** Initialize engine. Call this before any actions with engine. */
    virtual void init() = 0;

    /** Run main loop */
    virtual void run(const DESC& desc) = 0;

    /** Make single frame */
    virtual void frame() = 0;

    /** Stop main loop */
    virtual void stop() = 0;

    /** Check whether engine is working */
    virtual bool isRunning() const = 0;

    /** Get the number of the frame */
    virtual unsigned int getFrameNumber() const = 0;
    
    /** Get timer measuring time from start of the simulation. */
    virtual Timer& getSimulationTimer() = 0;

    /** Get simulation world */
    virtual realm::World* getWorld() = 0;

    /** Set world */
    virtual void setWorld(const realm::world_ptr& world_) = 0;

    /** Get database manager */
    virtual database::DatabaseManager& getDatabaseManager() = 0;

    /** Get graphics manager */
    virtual graphics::GraphicsManager& getGraphicsManager() = 0;

    /** Get input manager */
    virtual input::InputManager& getInputManager() = 0;

    /** Get log manager */
    virtual log::LogManager& getLogManager() = 0;

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Get physics manager */
    virtual physics::PhysicsManager& getPhysicsManager() = 0;
#endif

    /** Get thread manager */
    virtual thread::ThreadManager& getThreadManager() = 0;

	/** Get file system manager */
	virtual filesystem::FileSystemManager& getFileSystemManager() = 0;

    /** Get singleton engine instance */
    static Engine* Instance();

    /** Free engine */
    static void Free();

    virtual ~Engine() {}
};

} // namespace slon

#endif // __SLON_ENGINE_ENGINE_H__
