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
	{
		// write visual scenes
		ar.openChunk("VisualScenes");
		{
			typedef Library::key_visual_scene_map::const_iterator const_iterator;

			for (const_iterator it  = library->visualScenes.begin();
								it != library->visualScenes.end();
								++it)
			{
				ar.openChunk("VisualScene");
				ar.writeStringChunk("name", it->first.c_str(), it->first.length());
				ar.writeSerializable(it->second.get());
				ar.closeChunk();
			}
		}
		ar.closeChunk();
	}

    ar.writeToFile(*file);
}

} // namespace database
} // namespace slon