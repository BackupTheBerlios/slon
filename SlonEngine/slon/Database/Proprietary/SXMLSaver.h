#ifndef __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SAVER_H__
#define __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SAVER_H__

#include "../Saver.h"
#include "../Library.h"

namespace slon {
namespace database {

class SXMLSaver :
    public database::LibrarySaver
{
public:
    // Override database::Saver
    void save(library_ptr library, filesystem::File* file);
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_SAVER_H__
