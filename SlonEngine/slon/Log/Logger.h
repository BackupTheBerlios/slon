#ifndef __SLON_ENGINE_LOG_LOGGER__
#define __SLON_ENGINE_LOG_LOGGER__

#include "../Utility/referenced.hpp"
#include "LogManager.h"
#include "Forward.h"
#include <cassert>
#include <string>
#include <ostream>

/** Declare logger variable (use only in *.cpp file) with some stuff */
#ifdef SLON_ENGINE_DISABLE_AUTO_LOGGING
#	define DECLARE_AUTO_LOGGER(Name)
#else // !SLON_ENGINE_DISABLE_AUTO_LOGGING
#	define DECLARE_AUTO_LOGGER(Name)\
namespace\
{\
	const char*             autoLoggerName = Name;\
	slon::log::logger_ptr   autoLogger;\
	bool					autoLoggerReleased = false;\
	void ReleaseAutoLogger(slon::log::LogManager&)\
	{\
		autoLogger.reset();\
		autoLoggerReleased = true;\
	}\
}
#endif // !SLON_ENGINE_DISABLE_AUTO_LOGGING

#ifdef SLON_ENGINE_DISABLE_AUTO_LOGGING
#	define AUTO_LOGGER (slon::log::logger_ptr())
#else
#	define AUTO_LOGGER autoLogger 
#endif

/** Log message with specified severity. You this if you used DECLARE_AUTO_LOGGER("<name>") in your *.cpp file.
 * Example:
 * \code
 * AUTO_LOG_MESSAGE(S_FLOOD, "Hello world!");
 * AUTO_LOG_MESSAGE(S_NOTICE, "This is my " << 2 << " log message!");
 * \uncode
 */
#ifdef SLON_ENGINE_DISABLE_AUTO_LOGGING
#	define AUTO_LOGGER_MESSAGE(Severity, Message)
#else // !SLON_ENGINE_DISABLE_AUTO_LOGGING
#	define AUTO_LOGGER_MESSAGE(Severity, Message)\
{\
	assert( !autoLoggerReleased && "Trying to log message, but engine with log manager are released" );\
	if (!autoLogger && !autoLoggerReleased)\
	{\
		autoLogger = slon::log::currentLogManager().createLogger(autoLoggerName);\
		slon::log::currentLogManager().connectReleaseHandler(ReleaseAutoLogger);\
	}\
	(*autoLogger) << Severity << Message;\
}
#endif // !SLON_ENGINE_DISABLE_AUTO_LOGGING

#ifdef _DEBUG
#   define LOG_FILE_AND_LINE "; in" << __FILE__ << " at " << __LINE__ << " line\n"
#else
#   define LOG_FILE_AND_LINE "\n"
#endif

namespace slon {
namespace log {

/** Logger is simple ostream that redirects output
 * to the specified by LogManager destination. Loggers
 * are organized in tree structure. Id you redirect output
 * of the parent logger, all childs will be redirected too.
 */
class Logger :
	public Referenced
{
public:
    /** Get name of the logger. */
    virtual const std::string& getName() const = 0;

    /** Write warning level and log name, get ostream to write message */
    virtual std::ostream& operator << (SEVERITY severity) = 0;

    /** Flush ostream. */
    virtual void flush() = 0;

	virtual ~Logger() {}
};

} // namespace log
} // namespace slon

#endif // __SLON_ENGINE_LOG_LOGGER__
