#include "stdafx.h"
#include "Database/Proprietary/SXMLArchive.h"
#include "Database/Proprietary/SXMLSaver.h"
#include "FileSystem/File.h"
#include "Scene/Node.h"

DECLARE_AUTO_LOGGER("database.SXML")

namespace slon {
namespace database {

void SXMLSaver::save(library_ptr library, filesystem::File* file)
{
    // read file content
    if ( !file->open(filesystem::File::out) ) {
        throw file_error(AUTO_LOGGER, "Can't open *.sxml file for writing");
    }

    SXMLOArchive ar( getVersion(1, 0, 0) );
    ar.openChunk("VisualScenes");
    {
        typedef Library::key_visual_scene_array::const_iterator const_scene_iterator;

        for (const_scene_iterator it  = library->visualScenes.begin();
                                  it != library->visualScenes.end();
                                   ++it)
        {
            ar.openChunk("VisualScene");
            ar.writeStringChunk("name", it->first.c_str(), it->first.length());
            it->second->serialize(ar);
            ar.closeChunk();
        }
    }
    ar.closeChunk();

    ar.writeToFile(*file);
}

} // namespace database
} // namespace slon