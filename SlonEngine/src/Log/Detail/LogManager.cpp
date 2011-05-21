#include "stdafx.h"
#include "Detail/Engine.h"

namespace {

	using namespace slon::log::detail;
	
    std::string extractNextLevel(const std::string& baseName, const std::string& fullName)
    {
        using namespace std;

        size_t index = fullName.find_first_of( ".", baseName.length() + 1 );
        if (index == string::npos) {
            return fullName;
        }

        return fullName.substr(0, index);
    }

	logger_output* findNode(logger_output& loggerOutput, const std::string& name)
	{
		if (loggerOutput.name == name) {
			return &loggerOutput;
		}

		for(size_t i = 0; i<loggerOutput.children.size(); ++i)
		{
			logger_output* childFound = findNode(*loggerOutput.children[i], name);
			if (childFound) {
				return childFound;
			}
		}

		return 0;
	}

	void redirectChildrenOutput(logger_output& loggerOutput, const logger_output::ostream_ptr& os)
	{
		loggerOutput.os = os;
		for (size_t i = 0; i<loggerOutput.children.size(); ++i) {
			redirectChildrenOutput(*loggerOutput.children[i], os);
		}
	}


} // anonymous namespace

namespace slon {
namespace log {
namespace detail {

LogManager::LogManager()
:	mainLogger( new detail::Logger(logger_output_ptr(new logger_output)) )
{
}

LogManager::~LogManager()
{
	releaseSignal(*this);
}

bool LogManager::redirectOutput(const std::string& loggerName, const std::string& fileName)
{
    logger_output_ptr loggerOutput = findNode(*mainLogger->getLoggerOutput(), loggerName);
    if (!loggerOutput)
    {
        (*mainLogger) << log::S_ERROR << "Can't find requested logger: " << loggerName << std::endl;
        return false;
    }

    // create output
    loggerOutput->fb.reset(new std::filebuf);
    loggerOutput->fb->open( fileName.c_str(), std::ios::out );
    if ( !loggerOutput->fb->is_open() )
    {
        (*mainLogger) << log::S_ERROR << "Can't open output for logger: " << fileName << std::endl;
        return false;
    }

	logger_output::ostream_ptr os( new ostream( logger_sink(loggerOutput->fb.get()) ) );
    redirectChildrenOutput(*loggerOutput, os);

    if ( loggerName.empty() ) {
        (*mainLogger) << log::S_NOTICE << "Output for loggers redirected to file '"
                                       << fileName << "'" << std::endl;
    }
    else {
        (*mainLogger) << log::S_NOTICE << "Output for loggers '" << loggerName << "' redirected to file '"
                                       << fileName << "'" << std::endl;
    }

    return true;
}

bool LogManager::redirectOutputToConsole(const std::string& loggerName)
{
    logger_output_ptr loggerOutput = findNode(*mainLogger->getLoggerOutput(), loggerName);
    if (!loggerOutput)
    {
        (*mainLogger) << log::S_ERROR << "Can't find requested logger: " << loggerName << std::endl;
        return false;
    }

    // redirect output
    loggerOutput->fb.reset();
	
	logger_output::ostream_ptr os( new ostream( logger_sink(std::cout.rdbuf())) );
    redirectChildrenOutput(*loggerOutput, os);

    if ( loggerName.empty() ) {
        (*mainLogger) << log::S_NOTICE << "Output for loggers redirected to console" << std::endl;
    }
    else {
        (*mainLogger) << log::S_NOTICE << "Output for loggers '" << loggerName << "' redirected to console" << std::endl;
    }

    return true;
}

log::logger_ptr LogManager::createLogger(const std::string& name)
{
	return logger_ptr( new detail::Logger( getLoggerOutput(name) ) );
}

logger_output_ptr LogManager::getLoggerOutput(const std::string& name)
{
    logger_output_ptr mainOutput   = mainLogger->getLoggerOutput();
    logger_output_ptr parentOutput = mainOutput;
	logger_output_ptr loggerOutput;

    // create hierarchy until requested logger
    if ( logger_output* loggerOutputNode = findNode(*parentOutput, name) ) {
        loggerOutput.reset(loggerOutputNode);
    }
    else
    {
        std::string baseLevel, nextLevel;
        while ( ( nextLevel = extractNextLevel(baseLevel, name) ) != name )
        {
            logger_output* loggerOutput = findNode(*parentOutput, nextLevel);
            if (!loggerOutput) {
                parentOutput = logger_output_ptr( new logger_output(parentOutput.get(), nextLevel) );
            }
            else {
                parentOutput = loggerOutput;
            }

            baseLevel = nextLevel;
        }

        loggerOutput.reset( new logger_output(parentOutput.get(), name) );
    }

	return loggerOutput;
}

} // namespace detail

LogManager& currentLogManager()
{
    return Engine::Instance()->getLogManager();
}

} // namespace slon
} // namespace log
