#ifndef __SLON_ENGINE_DATABASE_LOADER_H__
#define __SLON_ENGINE_DATABASE_LOADER_H__

#include <istream>
#include "../Utility/error.hpp"
#include "../Utility/referenced.hpp"
#include "../FileSystem/Forward.h"
#include "Forward.h"

namespace slon {
namespace database {

/** Error occuring during loading of object */
class loader_error :
    public slon_error
{
public:
    loader_error( const log::logger_ptr&    logger,
                  const std::string&		message,
                  log::SEVERITY				severity = log::S_ERROR )
    :   slon_error(logger, message, severity)
    {}

	~loader_error() throw() {} 
};

/** Interface for database items loaders. */
template<typename T>
class SLON_PUBLIC Loader :
    public Referenced
{
public:
    /** Just load item. Throw loader_error if can't. 
     * @see loader_error
     */
    virtual T load(filesystem::File* file) = 0;

    virtual ~Loader() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_LOADER_H__
