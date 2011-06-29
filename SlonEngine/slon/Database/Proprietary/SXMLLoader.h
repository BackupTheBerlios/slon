#ifndef __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_LOADER_H__
#define __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_LOADER_H__

#include "../Loader.h"
#include "../Library.h"

namespace slon {
namespace database {

class SXMLLoader :
    public database::LibraryLoader
{
public:
    // Override database::Loader
    library_ptr load(filesystem::File* file);
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_PROPRIETARY_SXML_LOADER_H__
