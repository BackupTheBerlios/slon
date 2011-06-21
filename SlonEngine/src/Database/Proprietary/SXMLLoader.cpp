#include "stdafx.h"
#include "Database/Detail/Library.h"
#include "Database/Proprietary/SXMLLoader.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Log/Logger.h"
#include "FileSystem/File.h"
#include "Realm/Object.h"
#include <xml++/document.h>
#include <xml++/serialization/helpers.hpp>

DECLARE_AUTO_LOGGER("database.SXML")

namespace slon {
namespace database {

library_ptr SXMLLoader::load(filesystem::File* file)
{
    library_ptr library(new Library);

    // read file content
    if ( !file->open(filesystem::File::in) ) {
        throw file_error(AUTO_LOGGER, "Can't open *.sxml file for reading");
    }
	
    // serialize from xml
    SXMLIArchive ar;
	ar.readFromFile(*file);
	
	SXMLIArchive::chunk_info info;
	{
		// read visual scenes
		if ( ar.openChunk("VisualScenes", info) )
		{
			while ( ar.openChunk("VisualScene", info) )
			{
				std::string name;
				ar.readStringChunk("name", name);
				scene::node_ptr scene( dynamic_cast<scene::Node*>(ar.readSerializable()) );
				if (!scene) {
					throw serialization_error(AUTO_LOGGER, "Can't deserialize visual scene");
				}

				library->visualScenes.insert( std::make_pair(name, scene) );
				ar.closeChunk();
			}

			ar.closeChunk();
		}

		// read objects
		if ( ar.openChunk("Objects", info) )
		{
			while ( ar.openChunk("Object", info) )
			{
				std::string name;
				ar.readStringChunk("name", name);
				realm::object_ptr object( dynamic_cast<realm::Object*>(ar.readSerializable()) );
				if (!object) {
					throw serialization_error(AUTO_LOGGER, "Can't deserialize object");
				}

				library->objects.insert( std::make_pair(name, object) );
				ar.closeChunk();
			}

			ar.closeChunk();
		}
	}

    return library;
}

} // namespace database
} // namespace slon
