#ifndef __SLON_ENGINE_LOG_FORWARD__
#define __SLON_ENGINE_LOG_FORWARD__

#include "../Config.h"

namespace boost
{
    template<typename T>
    class intrusive_ptr;
}

namespace slon {
namespace log {

enum SEVERITY
{
	S_FLOOD,      /// Dump every usefull information
    S_NOTICE,     /// Just notify about some noticeable events
    S_WARNING,    /// Warn user
    S_ERROR,      /// Report an error that engine will resolve
    S_FATAL_ERROR /// Report an error that engine can't resolve
};

class Logger;
class LogManager;
class LogVisitor;

typedef boost::intrusive_ptr<Logger>		logger_ptr;
typedef boost::intrusive_ptr<const Logger>	const_logger_ptr;

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_FORWARD__