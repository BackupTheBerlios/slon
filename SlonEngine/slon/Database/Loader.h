#ifndef __SLON_ENGINE_DATABASE_LOADER_H__
#define __SLON_ENGINE_DATABASE_LOADER_H__

#include <istream>
#include "../Utility/error.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"

namespace slon {
namespace database {

/** Error occuring during loading of object */
class loader_error :
    public slon_error
{
public:
    loader_error( log::Logger&         logger,
                  const std::string&   message,
                  log::SEVERITY   severity = log::S_ERROR )
    :   slon_error(logger, message, severity)
    {}

	~loader_error() throw() {} 
};

/** Interface for database items loaders. */
template<typename T>
class Loader :
    public Referenced
{
public:
	/** Return tru if file need to be open in binary mode. */
	virtual bool binary() const = 0;

    /** Just load item. Throw loader_error if can't. 
     * @param source - source containing item file.
     * @see loader_error
     */
    virtual T load(std::istream& source) = 0;

    virtual ~Loader() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_LOADER_H__
