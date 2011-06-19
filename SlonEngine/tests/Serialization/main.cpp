#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Engine.h"
#include "FileSystem/File.h"
#include "Graphics/Effect/TransformEffect.h"
#include "Graphics/Renderable/Mesh.h"
#include "Graphics/Renderable/StaticMesh.h"
#ifdef SLON_ENGINE_USE_PHYSICS
#   include "Physics/PhysicsManager.h"
#endif
#include <boost/iostreams/stream_buffer.hpp>

#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/unit_test.hpp>

using namespace slon;

void writeLoadWriteCompare(database::library_ptr library)
{
	// Save & Load & Save
	database::saveLibrary("./test1.sxml", library);
	library = database::loadLibrary("./test1.sxml");
	database::saveLibrary("./test2.sxml", library);

	// Compare files
	{
		static const size_t buffer_size = 512;
		static char         buffer0[buffer_size];
		static char         buffer1[buffer_size];

		filesystem::file_ptr f1 = filesystem::asFile( filesystem::currentFileSystemManager().getNode("./test1.sxml") );
		filesystem::file_ptr f2 = filesystem::asFile( filesystem::currentFileSystemManager().getNode("./test2.sxml") );
		f1->open(filesystem::File::in);
		f2->open(filesystem::File::in);
		while ( !f1->eof() && !f2->eof() )
		{
			size_t s0 = f1->read(buffer0, buffer_size);
			size_t s1 = f2->read(buffer1, buffer_size);

			BOOST_CHECK_EQUAL(s0, s1);
			for (size_t i = 0; i<std::min(s0, s1); ++i) {
				BOOST_CHECK_EQUAL(buffer0[s0], buffer1[s1]);
			}
		}

		BOOST_CHECK( f1->eof() && f2->eof() );
	}
}

BOOST_AUTO_TEST_CASE(scene_serialization)
{
	using namespace scene;
	using namespace filesystem;

	Engine::Instance()->init();

	// make simple scene graph filled with random data
	group_ptr root( new Group("root") );
	root->addChild( new MatrixTransform("child_transform_1", math::make_scaling(0.5f, 1.0f, 2.0f)) );
	root->addChild( new MatrixTransform("child_transform_2", math::make_scaling(0.5f, 0.1f, 2.0f)) );

	group_ptr child( new Group("child") );
	root->addChild( child.get() );
	child->addChild( new Group("grandchild") );
	child->addChild( new MatrixTransform("grandchild_transform", math::make_scaling(2.0f, 2.0f, 2.0f)) );

	// Save & Load & Save
	database::library_ptr library(new database::Library);
	library->visualScenes.insert( std::make_pair("test", root) );
	writeLoadWriteCompare(library);
	
	// Clear cache for further tests
	database::currentDatabaseManager().clear(database::DatabaseManager::CLEAR_ALL);
}

BOOST_AUTO_TEST_CASE(mesh_serialization)
{
	using namespace graphics;
	using namespace filesystem;
	
    GraphicsManager& graphicsManager = currentGraphicsManager();
    graphicsManager.setVideoMode(640, 480, 32, false, false, 0);
	
	FFPRendererDesc desc;
    graphicsManager.initRenderer(desc);

	// create cube
	math::Vector3f	vertices[8];
	unsigned		indices[16];

    vertices[0] = math::Vector3f(-1.0f, -1.0f,  1.0f);
    vertices[1] = math::Vector3f(-1.0f,  1.0f,  1.0f);
    vertices[2] = math::Vector3f( 1.0f,  1.0f,  1.0f);
    vertices[3] = math::Vector3f( 1.0f, -1.0f,  1.0f);
    vertices[4] = math::Vector3f(-1.0f, -1.0f, -1.0f);
    vertices[5] = math::Vector3f(-1.0f,  1.0f, -1.0f);
    vertices[6] = math::Vector3f( 1.0f,  1.0f, -1.0f);
    vertices[7] = math::Vector3f( 1.0f, -1.0f, -1.0f);

    // Back face
    indices[0]  = 0;
    indices[1]  = 1;
    indices[2]  = 2;
    indices[3]  = 3;

    // Top face
    indices[4]  = 1;
    indices[5]  = 5;
    indices[6]  = 6;
    indices[7]  = 2;

    // Front face
    indices[8]  = 5;
    indices[9]  = 4;
    indices[10] = 7;
    indices[11] = 6;

    // Bottom face
    indices[12] = 4;
    indices[13] = 0;
    indices[14] = 3;
    indices[15] = 7;

	mesh_data_ptr mdata(new MeshData);
	mdata->setAttributes("position", 0, 3, 8, sgl::FLOAT, vertices);
	mdata->setIndices(0, 12, indices);

	// make mesh
	effect_ptr effect(new TransformEffect);
	mesh_ptr mesh( new Mesh(mdata) );
    mesh->addIndexedSubset(effect.get(), sgl::LINE_STRIP, 0,  4);
    mesh->addIndexedSubset(effect.get(), sgl::LINE_STRIP, 4,  4);
    mesh->addIndexedSubset(effect.get(), sgl::LINE_STRIP, 8,  4);
    mesh->addIndexedSubset(effect.get(), sgl::LINE_STRIP, 12, 4);

	// make scene
	static_mesh_ptr root( new StaticMesh(mesh.get()) );

	// check
	database::library_ptr library(new database::Library);
	library->visualScenes.insert( std::make_pair("test", root) );
	writeLoadWriteCompare(library);

	// Clear cache for further tests
	database::currentDatabaseManager().clear(database::DatabaseManager::CLEAR_ALL);
}

#ifdef SLON_ENGINE_USE_PHYSICS
BOOST_AUTO_TEST_CASE(physics_serialization)
{
    using namespace physics;

    // world
    PhysicsManager& physManager = currentPhysicsManager();
    DynamicsWorld*  dynWorld    = physManager.initDynamicsWorld();

    // physics scene
    rigid_body_ptr    rb0;
    rigid_body_ptr    rb1;
    physics_model_ptr model(new PhysicsModel);

    // rigid bodies
    {
        RigidBody::state_desc desc;
        desc.name           = "rb0";
        desc.collisionShape = new ConeShape(1.0f, 2.0f);
        desc.mass           = 1.0f;
        desc.target         = "node0";
        desc.transform      = math::make_translation(-1.0f, 0.0f, 0.0f);
        rb0 = dynWorld->createRigidBody(desc);
        model->addRigidBody(rb0.get());

        desc.name           = "rb1";
        desc.collisionShape = new BoxShape(0.5f, 0.5f, 0.5f);
        desc.mass           = 2.0f;
        desc.target         = "node1";
        desc.transform      = math::make_translation(1.0f, 0.0f, 0.0f);
        rb1 = dynWorld->createRigidBody(desc);
        model->addRigidBody(rb1.get());
    }

    // constraint
    {
        Constraint::state_desc desc;
        desc.name = "cons0";
        desc.rigidBodies[0]     = rb0.get();
        desc.rigidBodies[1]     = rb1.get();
        desc.angularLimits[0].x = -0.5f;
        desc.angularLimits[0].x =  0.5f;
        desc.frames[0]          = math::make_translation(1.0f, 0.0f, 0.0f);
        desc.frames[1]          = math::make_translation(-1.0f, 0.0f, 0.0f);
        model->addConstraint( dynWorld->createConstraint(desc) );
    }

    // scene
    scene::group_ptr root( new scene::MatrixTransform("node0") );
    root->addChild( new scene::MatrixTransform("node1") );

    realm::object_ptr object( realm::currentWorld().add(root.get(), true, model.get()) );

	// check
	database::library_ptr library(new database::Library);
	library->visualScenes.insert( std::make_pair("scene_test", root) );
    library->objects.insert( std::make_pair("obj_test", object) );
	writeLoadWriteCompare(library);

	// Clear cache for further tests
	database::currentDatabaseManager().clear(database::DatabaseManager::CLEAR_ALL);
}
#endif