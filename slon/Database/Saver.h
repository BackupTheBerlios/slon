#ifndef __SLON_ENGINE_DATABASE_SAVER_H__
#define __SLON_ENGINE_DATABASE_SAVER_H__

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
    saver_error( log::Logger&         logger,
                 const std::string&   message,
                 log::SEVERITY   severity = log::S_ERROR )
    :   slon_error(logger, message, severity)
    {}

	~saver_error() throw() {} 
};

/** Interface for database items loaders. */
template<typename T>
class Saver :
    public Referenced
{
public:
    /** Check wether need to open ostream in binary mode */
    virtual bool binary() const = 0;

    /** Just save item. Throw saver_error if can't. 
     * @param sink - sink where to dump item.
     * @see saver_error
     */
    virtual void save(T item, std::ostream& sink) = 0;

    virtual ~Saver() {}
};

} // namespace database
} // namespace slon

#endif // __SLON_ENGINE_DATABASE_SAVER_H__
