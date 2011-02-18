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

// construct output, redirect to cout
Logger::logger_output::logger_output()
:   os( new ostream( logger_sink( std::cout.rdbuf() ) ) )
{
    os->setf(std::ios_base::unitbuf);
}

// construct output, redirect to parent output
Logger::logger_output::logger_output(const logger_output_ptr& _parent, const std::string& _name) :
    parent(_parent),
    name(_name),
    os(_parent->os)
{
    parent->children.push_back(this);
}

Logger::logger_output::~logger_output()
{
    if (parent) {
        parent->children.erase( std::find(parent->children.begin(), parent->children.end(), this) );
    }
}

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

log::ostream& Logger::operator << (SEVERITY severity)
{
    using namespace slon::log;

    assert(loggerOutput);
    size_t flags = (*loggerOutput->os)->get_flags();
    if ( !(flags & logger_sink::SKIP_INFO) )
    {
    switch(severity)
    {
        case log::S_FLOOD:
            *(loggerOutput->os) << loggerOutput->name << " [flood] - ";
            break;

        case log::S_NOTICE:
            *(loggerOutput->os) << loggerOutput->name << " [notify] - ";
            break;

        case log::S_WARNING:
            *(loggerOutput->os) << loggerOutput->name << " [warning] - ";
            break;

        case log::S_ERROR:
            *(loggerOutput->os) << loggerOutput->name << " [error] - ";
            break;

        case log::S_FATAL_ERROR:
            *(loggerOutput->os) << loggerOutput->name << " [fatal error] - ";
            break;

        default:
            assert(!"Invalid case condition");
            break;
        }
    }

    return *(loggerOutput->os);
}

} // namesapce slon
} // namespace log
