#define _DEBUG_NEW_REDEFINE_NEW 0
#include "Engine.h"
#include "FileSystem/File.h"
#include <boost/iostreams/stream_buffer.hpp>

#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(scene_serialization)
{
	using namespace slon;
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
	database::saveLibrary("./test1.sxml", library);
	library = database::loadLibrary("./test1.sxml");
	database::saveLibrary("./test2.sxml", library);

	// Compare files
	{
		static const size_t buffer_size = 512;
		static char         buffer0[buffer_size];
		static char         buffer1[buffer_size];

		filesystem::file_ptr f1 = asFile( currentFileSystemManager().getNode("./test1.sxml") );
		filesystem::file_ptr f2 = asFile( currentFileSystemManager().getNode("./test2.sxml") );
		f1->open(File::in);
		f2->open(File::in);
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
