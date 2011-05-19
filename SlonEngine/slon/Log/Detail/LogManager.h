#ifndef __SLON_ENGINE_LOG_DETAIL_LOG_MANAGER__
#define __SLON_ENGINE_LOG_DETAIL_LOG_MANAGER__

#include "../LogManager.h"
#include "Logger.h"

namespace slon {
namespace log {
namespace detail {

/** Manages output for logs.
 */
class LogManager :
	public log::LogManager
{
public:
    LogManager();
	~LogManager();

	// Override LogManager
	bool			redirectOutput(const std::string& loggerName, const std::string& fileName);
	bool			redirectOutputToConsole(const std::string& loggerName);
	logger_ptr		createLogger(const std::string& name); 
    connection_type connectReleaseHandler(signal_type::slot_type slot) { return releaseSignal.connect(slot); }

    /** Get root or main logger of the LogManager */
    Logger& getMainLogger() { return mainLogger; }
	
    /** Get root or main logger of the LogManager */
	logger_output_ptr getLoggerOutput(const std::string& name);

private:
    Logger		mainLogger;
	signal_type	releaseSignal;
};

/** Get current input manager used by engine. */
LogManager& currentLogManager();

} // namespace detail
} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_DETAIL_LOG_MANAGER__
