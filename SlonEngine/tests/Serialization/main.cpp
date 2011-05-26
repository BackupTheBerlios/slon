#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Engine.h"

#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(scene_serialization)
{
	using namespace slon;
	using namespace scene;

	Engine::Instance()->init();

	// make simple scene graph filled with random data
	group_ptr root( new Group/*( hash_string("root") )*/ );
	root->addChild( new MatrixTransform(math::make_scaling(0.5f, 1.0f, 2.0f)) );
	root->addChild( new MatrixTransform(math::make_scaling(0.5f, 0.1f, 2.0f)) );

	group_ptr child( new Group/*( hash_string("child") )*/ );
	root->addChild( child.get() );
	child->addChild( new Group/*( hash_string("grandchild") )*/ );
	child->addChild( new MatrixTransform(math::make_scaling(2.0f, 2.0f, 2.0f)) );
    //root->addChild( child.get() );

	// Save it
	database::library_ptr library(new database::Library);
	library->visualScenes.push_back( std::make_pair("test", root) );

	database::saveLibrary("./test.sxml", library);

	// Clear
	root.reset();
	child.reset();

	// Load it
	library = database::loadLibrary("./test.sxml");
}
