#include "stdafx.h"
#include "Engine.h"

namespace slon {
namespace log {

Logger::logger_output* findNode(Logger::logger_output& loggerOutput, const std::string& name)
{
	if (loggerOutput.name == name) {
		return &loggerOutput;
	}

	for(size_t i = 0; i<loggerOutput.children.size(); ++i)
	{
		Logger::logger_output* childFound = findNode(*loggerOutput.children[i], name);
		if (childFound) {
			return childFound;
		}
	}

	return 0;
}

void redirectChildrenOutput(Logger::logger_output& loggerOutput, const boost::shared_ptr<ostream>& os)
{
    loggerOutput.os = os;
    for(size_t i = 0; i<loggerOutput.children.size(); ++i)
    {
        redirectChildrenOutput(*loggerOutput.children[i], os);
    }
}

LogManager::LogManager()
{
}

bool LogManager::redirectOutput(const std::string& loggerName, const std::string& fileName)
{
    Logger::logger_output* loggerOutput = findNode(*mainLogger.loggerOutput, loggerName);
    if (!loggerOutput)
    {
        mainLogger << log::S_ERROR << "Can't find requested logger: " << loggerName << std::endl;
        return false;
    }

    // create output
    loggerOutput->fb.reset(new std::filebuf);
    loggerOutput->fb->open( fileName.c_str(), std::ios::out );
    if ( !loggerOutput->fb->is_open() )
    {
        mainLogger << log::S_ERROR << "Can't open output for logger: " << fileName << std::endl;
        return false;
    }

	Logger::ostream_ptr os( new ostream( logger_sink(loggerOutput->fb.get()) ) );
    redirectChildrenOutput(*loggerOutput, os);

    if ( loggerName.empty() ) {
        mainLogger << log::S_NOTICE << "Output for loggers redirected to file '"
                                     << fileName << "'" << std::endl;
    }
    else {
        mainLogger << log::S_NOTICE << "Output for loggers '" << loggerName << "' redirected to file '"
                                     << fileName << "'" << std::endl;
    }

    return true;
}

bool LogManager::redirectOutputToConsole(const std::string& loggerName)
{
    Logger::logger_output* loggerOutput = findNode(*mainLogger.loggerOutput, loggerName);
    if (!loggerOutput)
    {
        mainLogger << log::S_ERROR << "Can't find requested logger: " << loggerName << std::endl;
        return false;
    }

    // redirect output
    loggerOutput->fb.reset();
	
	Logger::ostream_ptr os( new ostream( logger_sink(std::cout.rdbuf())) );
    redirectChildrenOutput(*loggerOutput, os);

    if ( loggerName.empty() ) {
        mainLogger << log::S_NOTICE << "Output for loggers redirected to console" << std::endl;
    }
    else {
        mainLogger << log::S_NOTICE << "Output for loggers '" << loggerName << "' redirected to console" << std::endl;
    }

    return true;
}

LogManager& currentLogManager()
{
    return Engine::Instance()->getLogManager();
}

} // namesapce slon
} // namespace log
