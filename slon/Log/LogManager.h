#ifndef __SLON_ENGINE_LOG_LOG_MANAGER__
#define __SLON_ENGINE_LOG_LOG_MANAGER__

#include "Logger.h"

namespace slon {
namespace log {

/** Manages output for logs.
 */
class LogManager
{
public:
    LogManager();

    /** Redirect output for the logger.
     * @param loggerName - name of the logger or group of loggers to redirect output.
     * If the name is "some_log", then all loggers with the names "some_log.log1",
     * "some_log.log2", "some_log.log.log" etc. will be redirected.
     * @param fileName - name of the file where to redirect.
     * @return result of the operation.
     * @see redirectOutputToConsole
     */
    bool redirectOutput(const std::string& loggerName, const std::string& fileName);

    /** Redirect output for logger into the console.
     * @param loggerName - name of the logger or group of loggers to redirect output.
     * If the name is "some_log", then all loggers with the names "some_log.log1",
     * "some_log.log2", "some_log.log.log" etc. will be redirected.
     * @return result of the operation.
     * @see redirectOutput
     */
    bool redirectOutputToConsole(const std::string& loggerName);

    /** Get root or main logger of the LogManager */
    Logger& getMainLogger() { return mainLogger; }

private:
    Logger mainLogger;
};

/** Get current input manager used by engine. */
LogManager& currentLogManager();

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_LOG_MANAGER__
