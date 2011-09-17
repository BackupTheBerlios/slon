#ifndef __SLON_ENGINE_LOG_LOG_MANAGER__
#define __SLON_ENGINE_LOG_LOG_MANAGER__

#include "Forward.h"
#include <boost/signals.hpp>
#include <string>

namespace slon {
namespace log {

/** Manages output for loggers. */
class SLON_PUBLIC LogManager
{
public:
    typedef boost::signal<void (LogManager&)>	signal_type;
    typedef boost::signals::connection			connection_type;

public:
    /** Redirect output for the logger.
     * @param loggerName - name of the logger or group of loggers to redirect output.
     * If the name is "some_log", then all loggers with the names "some_log.log1",
     * "some_log.log2", "some_log.log.log" etc. will be redirected.
     * @param fileName - name of the file where to redirect.
     * @return result of the operation.
     * @see redirectOutputToConsole
     */
    virtual bool redirectOutput(const std::string& loggerName, const std::string& fileName) = 0;

    /** Redirect output for logger into the console.
     * @param loggerName - name of the logger or group of loggers to redirect output.
     * If the name is "some_log", then all loggers with the names "some_log.log1",
     * "some_log.log2", "some_log.log.log" etc. will be redirected.
     * @return result of the operation.
     * @see redirectOutput
     */
    virtual bool redirectOutputToConsole(const std::string& loggerName) = 0;

	/** Create logger with specified name. Loggers are organized in tree structure.
	 * @see Logger
	 */
	virtual logger_ptr createLogger(const std::string& name) = 0;
	
	/** Attach handler for LogManager::Release event. */
    virtual connection_type connectReleaseHandler(signal_type::slot_type slot) = 0;

	virtual ~LogManager() {}
};

/** Get current input manager used by engine. */
SLON_PUBLIC LogManager& currentLogManager();

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_LOG_MANAGER__
