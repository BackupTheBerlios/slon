#include "stdafx.h"
#include "Log/LogManager.h"

namespace {

	using namespace slon;
	using namespace log;

    std::string extractNextLevel(const std::string& baseName, const std::string& fullName)
    {
        using namespace std;

        size_t index = fullName.find_first_of( ".", baseName.length() + 1 );
        if (index == string::npos) {
            return fullName;
        }

        return fullName.substr(0, index);
    }

} // anonymous namespace

namespace slon {
namespace log {

extern Logger::logger_output* findNode(Logger::logger_output& loggerOutput, const std::string& name);

Logger::Logger() :
    loggerOutput(new logger_output)
{
}

Logger::Logger(const std::string& name)
{
    logger_output_ptr mainOutput   = currentLogManager().getMainLogger().loggerOutput;
    logger_output_ptr parentOutput = mainOutput;

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
                parentOutput = logger_output_ptr( new logger_output(parentOutput, nextLevel) );
            }
            else {
                parentOutput.reset(loggerOutput);
            }

            baseLevel = nextLevel;
        }

        loggerOutput.reset( new logger_output(parentOutput, name) );
    }
}

std::ostream& Logger::operator << (WARNING_LEVEL warningLevel)
{
    using namespace slon::log;

    assert(loggerOutput);
    switch(warningLevel)
    {
    case log::WL_NOTIFY:
        *(loggerOutput->os) << loggerOutput->name << " [notify] - ";
        break;

    case log::WL_WARNING:
        *(loggerOutput->os) << loggerOutput->name << " [warning] - ";
        break;

    case log::WL_ERROR:
        *(loggerOutput->os) << loggerOutput->name << " [error] - ";
        break;

    case log::WL_FATAL_ERROR:
        *(loggerOutput->os) << loggerOutput->name << " [fatal error] - ";
        break;

    default:
        assert(!"Invalid case condition");
        break;
    }

    return *(loggerOutput->os);
}

} // namesapce slon
} // namespace log
