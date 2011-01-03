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

void redirectChildrenOutput(Logger::logger_output& loggerOutput, const boost::shared_ptr<std::ostream>& os)
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
    using namespace std;

    Logger::logger_output* loggerOutput = findNode(*mainLogger.loggerOutput, loggerName);
    if (!loggerOutput)
    {
        mainLogger << log::WL_ERROR << "Can't find requested logger: " << loggerName << endl;
        return false;
    }

    // create output
    loggerOutput->fb.reset(new filebuf);
    loggerOutput->fb->open( fileName.c_str(), ios::out );
    if ( !loggerOutput->fb->is_open() )
    {
        mainLogger << log::WL_ERROR << "Can't open output for logger: " << fileName << endl;
        return false;
    }
    redirectChildrenOutput( *loggerOutput, Logger::ostream_ptr( new ostream( loggerOutput->fb.get() ) ) );

    if ( loggerName.empty() ) {
        mainLogger << log::WL_NOTIFY << "Output for loggers redirected to file '"
                                     << fileName << "'" << endl;
    }
    else {
        mainLogger << log::WL_NOTIFY << "Output for loggers '" << loggerName << "' redirected to file '"
                                     << fileName << "'" << endl;
    }

    return true;
}

bool LogManager::redirectOutputToConsole(const std::string& loggerName)
{
    using namespace std;

    Logger::logger_output* loggerOutput = findNode(*mainLogger.loggerOutput, loggerName);
    if (!loggerOutput)
    {
        mainLogger << log::WL_ERROR << "Can't find requested logger: " << loggerName << endl;
        return false;
    }

    // redirect output
    loggerOutput->fb.reset();
    redirectChildrenOutput( *loggerOutput, Logger::ostream_ptr( new ostream( cout.rdbuf() ) ) );

    if ( loggerName.empty() ) {
        mainLogger << log::WL_NOTIFY << "Output for loggers redirected to console" << endl;
    }
    else {
        mainLogger << log::WL_NOTIFY << "Output for loggers '" << loggerName << "' redirected to console" << endl;
    }

    return true;
}

LogManager& currentLogManager()
{
    return Engine::Instance()->getLogManager();
}

} // namesapce slon
} // namespace log
