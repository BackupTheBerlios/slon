#include "stdafx.h"
#include "Database/Detail/Library.h"
#include "Database/Proprietary/SXMLLoader.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Log/Logger.h"
#include "FileSystem/File.h"
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
	if ( ar.openChunk("sxml", info) )
	{
		// read visual scenes
		if ( ar.openChunk("VisualScenes", info) )
		{
			while ( ar.openChunk("VisualScene", info) )
			{
				std::string name = readStringChunk(ar, "name");
				if ( !ar.openChunk("VisualScene", info) ) {
					throw serialization_error(AUTO_LOGGER, "Can't open visual scene chunk.");
				}

				scene::node_ptr scene( dynamic_cast<scene::Node*>(ar.readSerializableOrReference()) );
				if (!scene) {
					throw serialization_error(AUTO_LOGGER, "Can't deserialize visual scene.");
				}
				ar.closeChunk();

				library->visualScenes.insert( std::make_pair(name, scene) );
				ar.closeChunk();
			}

			ar.closeChunk();
		}

		ar.closeChunk();
	}

    return library;
}

} // namespace database
} // namespace slon
