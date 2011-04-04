#include "stdafx.h"
#include "Database/Detail/Library.h"
#include "Database/Proprietary/SXMLLoader.h"
#include "FileSystem/File.h"

namespace slon {
namespace database {

library_ptr SXMLLoader::load(filesystem::File* file)
{    
    // read file content
    if ( !file->open(filesystem::File::in) ) {
        throw file_error(logger, "Can't open *.sxml file for reading");
    }

	std::string fileContent(file->size(), ' ');
    file->read( &fileContent[0], fileContent.size() );
    file->close();

    // construct xml
    xmlpp::document document(fileContent.size(), fileContent.data());
    
    // serialize from xml
    SXMLSerializer serializer(document, xmlpp::LOAD);

    // construct library
    library_ptr library(new detail::Library);
    library->visualScenes = serializer.visualScenes;
#ifdef SLON_ENGINE_USE_PHYSICS
    library->physicsScenes = serializer.physicsScenes;
#endif
    return library;
}

} // namespace database
} // namespace slon
