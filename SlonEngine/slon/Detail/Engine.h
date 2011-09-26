#ifndef __SLON_ENGINE_DETAIL_ENGINE_H__
#define __SLON_ENGINE_DETAIL_ENGINE_H__

#include "../Engine.h"
#include "../Database/Detail/DatabaseManager.h"
#include "../Graphics/Detail/GraphicsManager.h"
#include "../Input/Detail/InputManager.h"
#include "../Log/Detail/LogManager.h"
#include "../FileSystem/Detail/FileSystemManager.h"
#include "../Realm/World.h"
#include "../Thread/Detail/ThreadManager.h"
#include "../Thread/StartStopTimer.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "../Physics/Detail/PhysicsManager.h"
#endif

namespace slon {
namespace detail {

/** Main class of the SlonEngine. The manager of the hole graphics,
 * physics, sound and etc in the scene.
 */
class Engine :
    public slon::Engine
{
public:
    typedef std::vector<scene::Node*> update_queue;

public:
    Engine();
    ~Engine();

	// Override Engine
    void run(const DESC& desc);
    void frame();
    void stop() { working = false; }

    bool isRunning() const { return working; }

    /** Get the number of the frame */
    unsigned int getFrameNumber() const { return frameNumber; }

    /** Get simulation world */
    realm::World* getWorld() { return world.get(); }

    /** Set world */
    void setWorld(const realm::world_ptr& world_) { world = world_; }

    /** Get timer measuring time from start of the simulation. */
    Timer& getSimulationTimer() { return *simulationTimer; }

    /** Get database manager */
    database::DatabaseManager& getDatabaseManager() { return databaseManager; }

    /** Get graphics manager */
    graphics::GraphicsManager& getGraphicsManager() { return graphicsManager; }

    /** Get input manager */
    input::InputManager& getInputManager()  { return inputManager; }

    /** Get log manager */
    log::LogManager& getLogManager() { return logManager; }

#ifdef SLON_ENGINE_USE_PHYSICS
    /** Get physics manager */
    physics::PhysicsManager& getPhysicsManager() { return physicsManager; }
#endif

    /** Get thread manager */
    thread::ThreadManager& getThreadManager() { return threadManager; }
	
	/** Get file system manager */
    filesystem::FileSystemManager& getFileSystemManager() { return *filesystemManager; }

    /** Add node to update quene */
    void addToUpdateQueue(scene::Node* node);

    /** Get singleton engine instance */
    static Engine* Instance();

private:
    void handleInput();
    void handlePhysics();
    void handlePhysicsCycle();
    void handleGraphics();
    void handleScene();

private:
    // managers, order is important!
    log::detail::LogManager                     logManager;
    thread::detail::ThreadManager               threadManager;
    input::detail::InputManager                 inputManager;
    graphics::detail::GraphicsManager           graphicsManager;
    database::detail::DatabaseManager           databaseManager;
    filesystem::detail::file_system_manager_ptr filesystemManager;
    realm::world_ptr						    world;
    start_stop_timer_ptr                        simulationTimer;
#ifdef SLON_ENGINE_USE_PHYSICS
    physics::detail::PhysicsManager             physicsManager;
#endif
    
    // node waiting update function to be called
    update_queue updateQueue;
    update_queue updateQueueTemp;
    boost::mutex updateQueueMutex;

    // misc
    DESC    desc;
    bool    working;
    int     frameNumber;

public:
    static Engine* engineInstance;
};

} // namespace detail
} // namespace slon

#endif // __SLON_ENGINE_DETAIL_ENGINE_H__
