#ifndef __SLON_ENGINE_DATABASE_SAVER_H__
#define __SLON_ENGINE_DATABASE_SAVER_H__

#include "../FileSystem/Forward.h"
#include "../Utility/error.hpp"
#include "../Utility/referenced.hpp"
#include "Forward.h"
#include <ostream>

namespace slon {
namespace database {

/** Error occuring during loading of object */
class saver_error :
    public slon_error
{
public:
    saver_error( const log::logger_ptr& logger,
                 const std::string&		message,
                 log::SEVERITY			severity = log::S_ERROR )
    :   slon_error(logger, message, severity)
    {}

	~saver_error() throw() {} 
};

/** Interface for database items loaders. */
template<typename T>
class SLON_PUBLIC Saver :
    public Referenced
{
public:
    /** Just save item. Throw saver_error if can't. 
     * @see saver_error
     */
    virtual void save(T item, filesystem::File* file) = 0;

    virtual ~Saver() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_SAVER_H__
