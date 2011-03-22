#include "Scene.h"
#include "Database/Collada.h"
#include "Graphics/Renderable/SkyBox.h"
#include "SceneGraph/TraverseVisitor.h"
#include <algorithm>
#include <string.h>

using namespace std;
using namespace net;
using namespace slon;

#undef min
#undef max

void Scene::CollectObjects(sg::Node* node, int playerId, bool ignoreMeshes, int collisionFlags)
{
	sg::MatrixTransform* transform = dynamic_cast<sg::MatrixTransform*>(node);
    if (transform) 
    {
        if ( transform->getName() == desc.canonName[0] )
        {
            canonId[0] = objects.size();

            scene_object object;
            object.transform = transform;
            object.mesh = NULL;
            object.collisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
            lowPriorityObjects.push_back( objects.size() );
            objects.push_back(object);

            ignoreMeshes = true;
        }
        else if ( transform->getName() == desc.canonName[1] ) 
        {
            canonId[1] = objects.size();

            scene_object object;
            object.transform = transform;
            object.mesh = NULL;
            object.collisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
            lowPriorityObjects.push_back( objects.size() );
            objects.push_back(object);

            ignoreMeshes = true;
        }
        else if ( transform->getName() == desc.kernelName[0] ) 
        {
            kernelId[0] = objects.size();

            scene_object object;
            object.transform = transform;
            object.mesh = NULL;
            object.collisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
            lowPriorityObjects.push_back( objects.size() );
            objects.push_back(object);

            return;
        }
        else if ( transform->getName() == desc.kernelName[1] ) 
        {
          
            kernelId[1] = objects.size();

            scene_object object;
            object.transform = transform;
            object.mesh = NULL;
            object.collisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
            lowPriorityObjects.push_back( objects.size() );
            objects.push_back(object);

            return;
        }
        else if ( transform->getName().find("Static") != transform->getName().npos )
        {
            collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
        }
        else if ( transform->getName() == "Ground-node" ) 
        {
            collisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
        }
        else if ( transform->getName() == "Water-node" )
        {
            return;
        }
        else if ( transform->getName() == desc.castleName[0] ) 
        {
            playerId = 0;
        }
        else if ( transform->getName() == desc.castleName[1] )
        {
            playerId = 1;
        }
    }

    graphics::Mesh* mesh = dynamic_cast<graphics::Mesh*>(node);
    if (mesh && !ignoreMeshes) 
    {
        scene_object object;
        object.transform = dynamic_cast<sg::MatrixTransform*>( node->getParent() );
        object.mesh = mesh;
        object.collisionFlags = collisionFlags;
        if ( playerId >= 0 ) 
        {
            object.valuable = true;
            object.playerId = playerId;
        }
        lowPriorityObjects.push_back( objects.size() );
        objects.push_back(object);
    }

    if ( sg::Group* group = node->asGroup() )
    {
	    for(size_t i = 0; i<group->getNumChildren(); ++i) {
		    CollectObjects( group->getChild(i), playerId, ignoreMeshes, collisionFlags );
	    }
    }
}

void Scene::ClearGraphics(sg::Node* node)
{
	graphics::Mesh* mesh = dynamic_cast<graphics::Mesh*>(node);
    if (mesh) {
        mesh->Clear();
    }
    
	graphics::SkyBox* skyBox = dynamic_cast<graphics::SkyBox*>(node);
    if (skyBox) {
        skyBox->Clear();
    }

    if ( sg::Group* group = node->asGroup() ) 
    {
	    for(size_t i = 0; i<group->getNumChildren(); ++i) {
		    ClearGraphics( group->getChild(i) );
	    }
    }
}

btTransform MakeBtTransform(const gmath::Matrix4f& matrix)
{
    btTransform result;
    result.setFromOpenGLMatrix(matrix.arr);
    return result;
}

gmath::Matrix4f MakeGMathMatrix(const btTransform& transform)
{
    gmath::Matrix4f result;
	transform.getOpenGLMatrix(result.arr);
    return result;
}

void BuildAABB(const graphics::Mesh* mesh, gmath::Vector3f& minVec, gmath::Vector3f& maxVec)
{
    using namespace gmath;

    const sgl::Mesh* sglMesh  = mesh->SglMesh();
    const Vector3f*  vertices = sglMesh->QueryVertexData<Vector3f>();

    minVec = Vector3f( 1e6f);
    maxVec = Vector3f(-1e6f);
    for(size_t i = 0; i<sglMesh->NumVertices(); ++i)
    {
        maxVec = gmath::max(vertices[i], maxVec);
        minVec = gmath::min(vertices[i], minVec);
    }
}

void BuildSphere(const graphics::Mesh* mesh, float& radius)
{
    using namespace gmath;

    const sgl::Mesh* sglMesh  = mesh->SglMesh();
    const Vector3f*  vertices = sglMesh->QueryVertexData<Vector3f>();

    radius = 0.0f;
    for(size_t i = 0; i<sglMesh->NumVertices(); ++i)
    {
        radius = std::max( radius, length(vertices[i]) );
    }
}


btTriangleMesh* BuildBtTriangleMesh(const gmath::Matrix4f& transform, const graphics::Mesh* mesh)
{
    using namespace sgl::math;

    const sgl::Mesh* sglMesh  = mesh->SglMesh();
    const Vector3f*  vertices = sglMesh->QueryVertexData<Vector3f>();
    const int*       indices  = sglMesh->VertexIndices();

    // copy data
    size_t          vertexDataSize = sizeof(Vector3f) * sglMesh->NumVertices();
    size_t          indexDataSize = sizeof(int) * sglMesh->NumVertexIndices();
    unsigned char*  vertexData = (unsigned char*)malloc(vertexDataSize);
    unsigned char*  indexData = (unsigned char*)malloc(indexDataSize);

    //memcpy(vertexData, vertices, vertexDataSize);
    Matrix4f transposed(transform);
    transposed.transpose();
    for(size_t i = 0; i<sglMesh->NumVertices(); ++i)
    {
        Vector3f* dstVertex = (Vector3f*)vertexData + i;
        Vector4f  vertex = transposed * Vector4f(vertices[i], 1.0f);
        *dstVertex = Vector3f(vertex.x, vertex.y, vertex.z);
    }
    memcpy(indexData, indices, indexDataSize);

    // create indexed mesh
    btIndexedMesh indexedMesh;
    indexedMesh.m_numVertices         = sglMesh->NumVertices();
    indexedMesh.m_vertexBase          = vertexData;
    indexedMesh.m_vertexStride        = sizeof(Vector3f);
    indexedMesh.m_triangleIndexBase   = indexData;
    indexedMesh.m_triangleIndexStride = 3 * sizeof(int);
    indexedMesh.m_numTriangles        = sglMesh->NumVertexIndices() / 3;
    indexedMesh.m_indexType           = PHY_INTEGER;

    // create mesh
    btTriangleMesh* triangleMesh = new btTriangleMesh();
    triangleMesh->addIndexedMesh(indexedMesh);

    return triangleMesh;
}

gmath::Vector3f RemoveScaling(gmath::Matrix4f& mat)
{
    using namespace gmath;

    Vector3f scale; 
    for(int i = 0; i<3; ++i)
    {
        Vector3f vec(mat.mat[i][0], mat.mat[i][1], mat.mat[i][2]);
        scale[i] = length(vec);
        vec = normalize(vec);
        mat.mat[i][0] = vec.x;
        mat.mat[i][1] = vec.y;
        mat.mat[i][2] = vec.z;
    }

    return scale;
}

btRigidBody* Scene::scene_object::CreateRigidBody(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld, float mass)
{
    if ( collisionFlags & btCollisionObject::CF_STATIC_OBJECT 
         || collisionFlags & btCollisionObject::CF_KINEMATIC_OBJECT )
    {
        btRigidBody::btRigidBodyConstructionInfo rbInfo( 0.0f,
                                                         motionState.get(),
                                                         shape.get(),
                                                         btVector3(0.0f, 0.0f, 0.0f) );
        rbInfo.m_friction = 2.0f;
        rigidBody.reset( new btRigidBody(rbInfo) );
    }
    else 
    {
        if ( mass < 20.0f ) {
            mass = 20.0f;
        }
        else if ( mass > 500.0f ) {
            mass = 500.0f;
        }

        btVector3 inertia;
        shape->calculateLocalInertia(mass, inertia); 
        btRigidBody::btRigidBodyConstructionInfo rbInfo( mass,
                                                         motionState.get(),
                                                         shape.get(),
                                                         inertia );
        rbInfo.m_friction = 1.0f;
        rigidBody.reset( new btRigidBody(rbInfo) );
    }
    rigidBody->setCollisionFlags( rigidBody->getCollisionFlags() | collisionFlags );
    pDynamicsWorld = _pDynamicsWorld;
    pDynamicsWorld->addRigidBody( rigidBody.get() );

    // get origin
    btTransform btTrans;
    motionState->getWorldTransform(btTrans);
    btVector3 vec = btTrans.getOrigin();
    origin = gmath::Vector3f( vec.x(), vec.y(), vec.z() );

    return rigidBody.get();
}

btRigidBody* Scene::scene_object::CreateBox(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld)
{
    using namespace gmath;

    gmath::Matrix4f localToWorld = transform->getLocalToWorldTransform();
    Vector3f scale = RemoveScaling(localToWorld);

    // create shape
    Vector3f    minVec, maxVec;
    if (mesh) 
    {
        BuildAABB(mesh, minVec, maxVec);
        Vector3f halfExtents = (maxVec - minVec) * scale / 2.0f;
        shape.reset( new btBoxShape( btVector3(halfExtents.x, halfExtents.y, halfExtents.z) ) );
    }
    else {
        shape.reset( new btBoxShape( btVector3(1.0f, 1.0f, 1.0f) ) );
    }

    // create  construction info
    Vector3f center = Vector3f(minVec + maxVec) * scale / 2.0f;
    transformOffset = Mat4f::scale(scale) * Mat4f::translate(-center);
    Matrix4f invTranslate = Mat4f::translate(center);
    motionState.reset( new btDefaultMotionState( MakeBtTransform(invTranslate * localToWorld) ) );

    return CreateRigidBody(_pDynamicsWorld, length(maxVec - minVec) * 100.0f);
}

btRigidBody* Scene::scene_object::CreateCylinder(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld)
{
    using namespace gmath;

    gmath::Matrix4f localToWorld = transform->getLocalToWorldTransform();
    Vector3f scale = RemoveScaling(localToWorld);

    // create shape
    Vector3f minVec, maxVec;
    if (mesh) 
    {
        BuildAABB(mesh, minVec, maxVec);
        Vector3f halfExtents = (maxVec - minVec) * scale / 2.0f;
        shape.reset( new btCylinderShapeZ( btVector3(halfExtents.x, halfExtents.y, halfExtents.z) ) );
    }
    else {
        shape.reset( new btCylinderShapeZ( btVector3(1.0f, 1.0f, 1.0f) ) );
    }

    // create  construction info
    Vector3f center = Vector3f(minVec + maxVec) * scale / 2.0f;
    transformOffset = Mat4f::scale(scale) * Mat4f::translate(-center);
    Matrix4f invTranslate = Mat4f::translate(center);
    motionState.reset( new btDefaultMotionState( MakeBtTransform(invTranslate * localToWorld) ) );

    return CreateRigidBody(_pDynamicsWorld, length(maxVec - minVec) * 100.0f);
}

btRigidBody* Scene::scene_object::CreateSphere(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld)
{
    using namespace gmath;

    gmath::Matrix4f localToWorld = transform->getLocalToWorldTransform();
    Vector3f scale = RemoveScaling(localToWorld);

    // create shape
    float radius = 1.1f;
    if (mesh) {
        BuildSphere(mesh, radius);
    }
    shape.reset( new btSphereShape(radius) );

    // create  construction info
    transformOffset = Mat4f::scale(scale);
    motionState.reset( new btDefaultMotionState( MakeBtTransform(localToWorld) ) );

    return CreateRigidBody(_pDynamicsWorld, 20.0f);
}

btRigidBody* Scene::scene_object::CreateBVH(const boost::shared_ptr<btDiscreteDynamicsWorld>& _pDynamicsWorld)
{
    // create shape
    btTriangleMesh* triangleMesh = BuildBtTriangleMesh(transform->getLocalToWorldTransform(), mesh);

    // create rigid body
    motionState.reset( new btDefaultMotionState() );
    shape.reset( new btBvhTriangleMeshShape(triangleMesh, true) );

    return CreateRigidBody(_pDynamicsWorld, 1000.0f);
}
/*
btRigidBody* Scene::CreateConvexHull(const scene_object& object)
{
    // create shape
    btTriangleMesh*         triangleMesh = BuildBtTriangleMesh(object.transform->LocalToWorldTransform(), object.mesh);
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);

    // create rigid body
    btDefaultMotionState* defaultMotionState = new btDefaultMotionState();
    btRigidBody::btRigidBodyConstructionInfo rbInfo( 100.0,
                                                     defaultMotionState,
                                                     shape,
                                                     btVector3(0.0, 0.0, 0.0) );
    rbInfo.m_friction = 0.5f;

    return new btRigidBody(rbInfo);
}
*/
void Scene::Reset(const scene_desc& _desc)
{
    desc = _desc;

	//galleryMesh.LoadFromColladaFile("data/Models/torus.DAE", "Torus_Knot01");
    sceneRoot.reset( new sg::MatrixTransform() );

	cerr << "starting scene loading" << endl;
	
    db::LoadColladaScene(desc.sceneFileName, *sceneRoot);

	// light
	//sgl::Light* light = pEngine->sglDevice()->CreateLight();
	//light->SetAmbient( Vector4f(0.0f, 0.0f, 0.0f, 0.0f) );
	//light->SetDiffuse( Vector4f(0.7f, 0.7f, 0.7f, 1.0f) );
	//light->SetPosition( Vector4f(-1000.0f, 1000.0f, 600.0f, 1.0f) ) ;

    // store objects
    sg::TraverseVisitor visitor;
    sceneRoot->accept(visitor);
    CollectObjects(sceneRoot.get(), -1, false, 0);


    // create physics scene
    btVector3	worldAabbMin(-1000, -100, -1000);
    btVector3	worldAabbMax(1000, 300, 1000);
    int			maxProxies = 2048;

    pBroadphase.reset( new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies) );

    pCollisionConfiguration.reset( new btDefaultCollisionConfiguration() );

    pCollisionDispatcher.reset( new btCollisionDispatcher( pCollisionConfiguration.get() ) );

    pImpulseSolver.reset( new btSequentialImpulseConstraintSolver() );

    pDynamicsWorld.reset( new btDiscreteDynamicsWorld( pCollisionDispatcher.get(),
			   									       pBroadphase.get(),
												       pImpulseSolver.get(),
												       pCollisionConfiguration.get() ) );

    // create rigid bodies for nodes
    for(size_t i = 0; i<objects.size(); ++i)
    {
        std::string objectName = objects[i].transform->getName();
        if ( objects[i].transform->getName() == "Ground-node" ) 
        {
            objects[i].CreateBVH(pDynamicsWorld);
            objects[i].rigidBody->setActivationState(DISABLE_DEACTIVATION);
            objects[i].lastSyncTime = 1e10;
        }
        else if ( i == canonId[0] || i == canonId[1] ) 
        {
            // kinematic canons
            objects[i].CreateBox(pDynamicsWorld);
        }
        else if ( i == kernelId[0] || i == kernelId[1] ) 
        {
            // kinematic canons
            objects[i].CreateSphere(pDynamicsWorld);
        }
        else if ( objectName.find("wall") != objectName.npos ) 
        {
            objects[i].CreateBox(pDynamicsWorld);
        }
        else {
            objects[i].CreateCylinder(pDynamicsWorld);
        }

        // Transform matrix is in world
        objects[i].transform->setTransformMatrix( objects[i].transform->getLocalToWorldTransform() );
    }

    // remove graphics scene
    ClearGraphics( sceneRoot.get() );

    // Reset health
    lastUpdatedObject = -1;
    initialHealth = 67;
    playerHealth[0] = playerHealth[1] = 100;

	cerr << "scene created" << endl;
}

void Scene::TranslateToClient(tcp::socket& socket, int playerId)
{
    // translate sky box
    sky_box_desc skyDesc;
    std::copy(desc.skyBoxMaps, desc.skyBoxMaps + 6, skyDesc.maps);
    skyDesc.write(socket);

    // translate level mesh
    mesh_desc meshDesc;
    meshDesc.fileName = desc.sceneFileName;
    meshDesc.write(socket);

    // translate light
    
    // translate canon
    canon_desc canonDesc;
    canonDesc.id = canonId[playerId];
    canonDesc.write(socket);

    // translate kernel
    kernel_desc kernelDesc;
    kernelDesc.id = kernelId[playerId];
    kernelDesc.write(socket);

    // translate objects
    for(size_t i = 0; i<objects.size(); ++i) 
    {
        object_desc objDesc;
        objDesc.id = i;
        objDesc.clone = false;
        objDesc.nodeName = objects[i].transform->getName();
        objDesc.matrix = objects[i].transform->getLocalToWorldTransform();
        objDesc.write(socket);
    }
}

void Scene::UpdateObjects(const object_transform_desc& transformDesc)
{
    //objects[transformDesc.id].matrixTransform = transformDesc.matrix;
    scene_object& object = objects[transformDesc.id];
    object.transform->setTransformMatrix(transformDesc.matrix);
    object.rigidBody->setWorldTransform( MakeBtTransform(transformDesc.matrix) );
}

std::string Scene::NeccessaryMessage()
{
    std::string message;
    for(size_t i = 0; i < objectsToTranslate.size(); ++i) 
    {
        message += objectsToTranslate[i].message();
    }

    return message;
}

void Scene::ClearNecessaryMessage()
{
    objectsToTranslate.clear();
}

bool Scene::HaveNecessaryData()
{
    return objectsToTranslate.size() > 0;
}
/*
void Scene::SynchroniseWithPlayer(net::tcp::socket& socket, float time)
{
    // first - high priority
    for(int j = highPriorityObjects.size() - 1; j>=0; --j) 
    {
        int i = highPriorityObjects[j];
        if ( /*(time - objects[i].lastSyncTime > min_transmission_interval)
             && objects[i].rigidBody->isActive() && objects[i].rigidBody->isInWorld()
                  || objects[i].rigidBody->isKinematicObject() ) 
        {
            objects[i].lastSyncTime = time;

            object_transform_desc desc;
            desc.id = i;
            if ( objects[i].rigidBody->isKinematicObject() ) {
                desc.matrix = objects[i].transform->TransformMatrix();
            }
            else {
                desc.matrix = objects[i].transformOffset
                              * MakeGMathMatrix( objects[i].rigidBody->getWorldTransform() );
            }
            desc.write(socket);
        }
    }

    //cerr << lastUpdatedObject << " " << objects.size() << endl;
    // low priority
    for(size_t j = 0; j<lowPriorityObjects.size(); ++j) 
    {
        lastUpdatedObject = j;
        int i = lowPriorityObjects[j];
        if ( (time - objects[i].lastSyncTime > min_transmission_interval)
              && ( objects[i].rigidBody->isActive() //&& !objects[i].rigidBody->wantsSleeping()
                   || objects[i].rigidBody->isKinematicObject()) ) 
        {
            object_transform_desc desc;
            desc.id = i;
            if ( objects[i].rigidBody->isKinematicObject() ) {
                desc.matrix = objects[i].transform->TransformMatrix();
            }
            else {
                desc.matrix = objects[i].transformOffset
                              * MakeGMathMatrix( objects[i].rigidBody->getWorldTransform() );
            }

            int k;
            for(k = 0; k<16; ++k) 
            {
                if ( fabs(objects[i].lastTransmitted.arr[k] - desc.matrix.arr[k]) > 1e-2 ) 
                {
                    objects[i].lastSyncTime = time;
                    objects[i].lastTransmitted = desc.matrix; 
                    desc.write(socket);
 
                    if ( objects[i].valuable && length(desc.matrix.translation() - objects[i].origin) > 3.0f ) 
                    {
                        objects[i].valuable = false;
                        playerHealth[ objects[i].playerId ] -= 1;
                    }

                    break;
                }
            }
        }
    }
}
*/
std::string Scene::MakeMessage(float time)
{
    std::string message;

    // first - high priority
    for(int j = highPriorityObjects.size() - 1; j>=0; --j) 
    {
        int i = highPriorityObjects[j];
        if ( objects[i].rigidBody->isActive() && objects[i].rigidBody->isInWorld()
             || objects[i].rigidBody->isKinematicObject() ) 
        {
            objects[i].lastSyncTime = time;

            object_transform_desc desc;
            desc.id = i;
            if ( objects[i].rigidBody->isKinematicObject() ) {
                desc.matrix = objects[i].transform->getTransformMatrix();
            }
            else {
                desc.matrix = objects[i].transformOffset
                              * MakeGMathMatrix( objects[i].rigidBody->getWorldTransform() );
            }
            message += desc.message();
        }
    }

    //cerr << lastUpdatedObject << " " << objects.size() << endl;
    // low priority
    for(size_t j = 0; j<lowPriorityObjects.size(); ++j) 
    {
        lastUpdatedObject = j;
        int i = lowPriorityObjects[j];
        if ( objects[i].rigidBody->isActive() 
             || objects[i].rigidBody->isKinematicObject() ) 
        {
            object_transform_desc desc;
            desc.id = i;
            if ( objects[i].rigidBody->isKinematicObject() ) {
                desc.matrix = objects[i].transform->getTransformMatrix();
            }
            else {
                desc.matrix = objects[i].transformOffset
                              * MakeGMathMatrix( objects[i].rigidBody->getWorldTransform() );
            }

            int k;
            for(k = 0; k<16; ++k) 
            {
                if ( fabs(objects[i].lastTransmitted.arr[k] - desc.matrix.arr[k]) > 1e-3 ) 
                {
                    objects[i].lastSyncTime = time;
                    objects[i].lastTransmitted = desc.matrix; 
                    message += desc.message();
 
                    if ( objects[i].valuable && length(desc.matrix.translation() - objects[i].origin) > 3.0f ) 
                    {
                        objects[i].valuable = false;
                        playerHealth[ objects[i].playerId ] -= 1;
                    }

                    break;
                }
            }
        }
    }

    return message;
}

void Scene::AdvanceSimulation(float time)
{
    static float lastUpdateTime = time;
    float dt = std::min(time - lastUpdateTime, 0.1f);
    //if ( dt >= 1.0 / 60.0f ) 
    //{
        pDynamicsWorld->stepSimulation(dt);
        lastUpdateTime = time;
   // }
}

void Scene::FireCanon(float impulse, int playerId)
{
    using namespace gmath;

    scene_object* kernel = &objects[ kernelId[playerId] ];
    scene_object* canon = &objects[ canonId[playerId] ];

    Matrix4f transform = canon->transform->getTransformMatrix();
    gmath::Vector3f scale = RemoveScaling(transform);
    Vector3f impulseVec(transform.mat[2][0], transform.mat[2][1], transform.mat[2][2]);

    // create node
    sg::MatrixTransform* newKernelNode = new sg::MatrixTransform();
    sceneRoot->addChild(newKernelNode);

    scene_object object;
    btTransform btTrans = MakeBtTransform(transform);
    btTrans.setOrigin( btTrans.getOrigin() + btVector3(impulseVec.x, impulseVec.y, impulseVec.z) * 5.0f );
    object.motionState.reset( new btDefaultMotionState(btTrans) );
    object.shape = kernel->shape;//reset( new btSphereShape(0.3f) );
    object.mesh = NULL;   
    object.transformOffset = kernel->transformOffset;
    object.transform = newKernelNode;
    object.CreateRigidBody(pDynamicsWorld, 350.0f);

    // apply impulse
    impulseVec *= impulse;
    object.rigidBody->applyCentralImpulse( btVector3(impulseVec.x, impulseVec.y, impulseVec.z) );

    // enqueue object to translation queue
    object_desc kernelDesc;
    kernelDesc.id = objects.size();
    kernelDesc.clone = true;
    kernelDesc.nodeName = kernel->transform->getName();
    kernelDesc.matrix = MakeGMathMatrix(btTrans);
    objectsToTranslate.push_back(kernelDesc);    

    // add object
    highPriorityObjects.push_back( objects.size() );
    objects.push_back(object);
}