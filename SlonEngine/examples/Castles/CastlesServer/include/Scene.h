#ifndef SLON_ENGINE_CASTLES_SCENE_H
#define SLON_ENGINE_CASTLES_SCENE_H

#include "Network.h"
#include "Graphics/Renderable/Mesh.h"
#include "SceneGraph/MatrixTransform.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

/*
#include <Extras/BulletMultiThreaded/SpuParallelSolver.h>
#include <Extras/BulletMultiThreaded/SpuSolverTask/SpuParallellSolverTask.h>
#include <Extras/BulletMultiThreaded/SpuGatheringCollisionDispatcher.h>
#include <Extras/BulletMultiThreaded/PlatformDefinitions.h>
#include <Extras/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h>
#if defined (WIN32)
#include <Extras/BulletMultiThreaded/Win32ThreadSupport.h>
#elif defined (USE_PTHREADS)
#include <Extras/BulletMultiThreaded/PosixThreadSupport.h>
#endif
*/

class Scene
{
public:
    struct scene_desc
    {
	    std::string     skyBoxMaps[6];
        std::string     sceneFileName;
        std::string     kernelName[2];
        std::string     canonName[2];
        std::string     castleName[2];
    };

    struct scene_object
    {
        float                       lastSyncTime;
        slon::sg::MatrixTransform*  transform;
        gmath::Matrix4f             lastTransmitted;
        gmath::Matrix4f             transformOffset;
        gmath::Vector3f             origin;
        int                         collisionFlags;
        int                         playerId;
        bool                        valuable;
        slon::graphics::Mesh*       mesh;

        // physics
        boost::shared_ptr<btDiscreteDynamicsWorld>	pDynamicsWorld;
        boost::shared_ptr<btRigidBody>              rigidBody;
        boost::shared_ptr<btCollisionShape>         shape;
        boost::shared_ptr<btDefaultMotionState>     motionState;

        scene_object() :
            lastSyncTime(0.0f),
            collisionFlags(0),
            valuable(false)
        {}

        ~scene_object()
        {
            if ( pDynamicsWorld.get() && rigidBody.get() && rigidBody.use_count() == 1 )
            {
                pDynamicsWorld->removeRigidBody( rigidBody.get() );
            }
        }

        btRigidBody* CreateRigidBody(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld, float mass);
        btRigidBody* CreateBox(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld);
        btRigidBody* CreateCylinder(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld);
        btRigidBody* CreateSphere(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld);
        btRigidBody* CreateBVH(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld);
    };

    typedef std::vector<scene_object> object_array;

    void Reset(const scene_desc& _desc);

    void TranslateToClient(net::tcp::socket& socket, int playerId);

    void UpdateObjects(const net::object_transform_desc& transformDesc);

    std::string NeccessaryMessage();

    void ClearNecessaryMessage();

    bool HaveNecessaryData();

    void SynchroniseWithPlayer(net::tcp::socket& socket, float time);

    std::string MakeMessage(float time);

    void AdvanceSimulation(float time);

    void FireCanon(float impulse, int playerId);

    int PlayerHealth(int playerId) const { return (int)(playerHealth[playerId] * (100.0f / initialHealth)); }

private:
    void CollectObjects(slon::sg::Node* node, int playerId, bool ignoreMeshes, int collisionFlags);

    void ClearGraphics(slon::sg::Node* node);

private:
    // desc
    scene_desc desc;

    // network
    int  lastUpdatedObject;

    // physics
	boost::shared_ptr<btDiscreteDynamicsWorld>			    pDynamicsWorld;
	boost::shared_ptr<btSequentialImpulseConstraintSolver>  pImpulseSolver;
	boost::shared_ptr<btAxisSweep3>					        pBroadphase;
    boost::shared_ptr<btCollisionDispatcher>		        pCollisionDispatcher;
    boost::shared_ptr<btDefaultCollisionConfiguration>		pCollisionConfiguration;

    // scene objects
    boost::shared_ptr<slon::sg::Group> sceneRoot;
    int                                initialHealth;
    int                                canonId[2];
    int                                kernelId[2];
    int                                playerHealth[2];
    object_array                       objects;
    std::vector<int>                   lowPriorityObjects;
    std::vector<int>                   highPriorityObjects;
    std::vector<net::object_desc>      objectsToTranslate;
};

#endif // SLON_ENGINE_CASTLES_SCENE_H
